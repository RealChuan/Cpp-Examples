# -*- coding: utf-8 -*-

from flask import Flask, request, jsonify, send_from_directory
import os
import gzip
from datetime import datetime
import glob

app = Flask(__name__)

# 配置上传目录
UPLOAD_FOLDER = "./crash_reports"
os.makedirs(UPLOAD_FOLDER, exist_ok=True)


def decompress_gzip_data(data):
    """解压 gzip 数据"""
    try:
        return gzip.decompress(data)
    except Exception as e:
        print(f"解压 gzip 数据失败: {str(e)}")
        return None


def save_compressed_file(guid, raw_data):
    """保存压缩的崩溃文件"""
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"crash_{timestamp}_{guid}.dmp.gz"
    filepath = os.path.join(UPLOAD_FOLDER, filename)

    with open(filepath, "wb") as f:
        f.write(raw_data)

    return filename, filepath


def save_decompressed_file(guid, decompressed_data):
    """保存解压后的崩溃文件"""
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"crash_{timestamp}_{guid}.dmp"
    filepath = os.path.join(UPLOAD_FOLDER, filename)

    with open(filepath, "wb") as f:
        f.write(decompressed_data)

    return filename, filepath


def save_regular_file(guid, file):
    """保存普通文件上传"""
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"crash_{timestamp}_{guid}.dmp"
    filepath = os.path.join(UPLOAD_FOLDER, filename)

    file.save(filepath)
    return filename, filepath


def handle_gzip_upload(guid, raw_data):
    """处理 gzip 压缩的上传"""
    decompressed_data = decompress_gzip_data(raw_data)

    if not decompressed_data:
        return None, "Failed to decompress gzip data"

    # 保存压缩文件
    compressed_filename, compressed_filepath = save_compressed_file(guid, raw_data)
    compressed_size = os.path.getsize(compressed_filepath)

    # 保存解压文件
    decompressed_filename, decompressed_filepath = save_decompressed_file(
        guid, decompressed_data
    )
    decompressed_size = os.path.getsize(decompressed_filepath)

    print(f"成功保存压缩崩溃文件: {compressed_filename} ({compressed_size} bytes)")
    print(f"成功保存解压崩溃文件: {decompressed_filename} ({decompressed_size} bytes)")

    return {
        "compressed_file": compressed_filename,
        "decompressed_file": decompressed_filename,
        "compressed_size": compressed_size,
        "decompressed_size": decompressed_size,
    }, None


def handle_regular_upload(guid, request):
    """处理普通文件上传"""
    if not request.files:
        return None, "No file data found"

    file_field_name = list(request.files.keys())[0]
    file = request.files[file_field_name]

    filename, filepath = save_regular_file(guid, file)
    file_size = os.path.getsize(filepath)

    print(f"成功保存崩溃文件: {filename} ({file_size} bytes)")

    return {
        "filename": filename,
        "size": file_size,
    }, None


@app.route("/upload", methods=["POST"])
def upload_crash():
    """
    接收 Crashpad 上传的崩溃文件
    """
    print(f"[{datetime.now()}] 收到崩溃上报请求")
    print(f"方法: {request.method}")
    print(f"内容类型: {request.content_type}")
    print(f"头部: {dict(request.headers)}")
    print(f"查询参数: {dict(request.args)}")

    # 获取 GUID 参数
    guid = request.args.get("guid", "unknown")

    try:
        # 检查是否是 gzip 压缩的内容
        if request.headers.get("Content-Encoding") == "gzip":
            print("检测到 gzip 压缩内容，正在处理...")
            raw_data = request.get_data()
            result, error = handle_gzip_upload(guid, raw_data)
        else:
            print("非 gzip 压缩内容，尝试普通文件处理")
            print(f"文件字段: {list(request.files.keys())}")
            result, error = handle_regular_upload(guid, request)

        if error:
            return jsonify({"error": error}), 400

        # 返回成功响应
        response_data = {
            "status": "success",
            "message": "Crash report uploaded successfully",
            "guid": guid,
        }
        response_data.update(result)

        return jsonify(response_data)

    except Exception as e:
        print(f"处理上传失败: {str(e)}")
        return jsonify({"error": f"Failed to process upload: {str(e)}"}), 500


def get_file_info():
    """获取文件列表信息"""
    files = []
    for filepath in glob.glob(os.path.join(UPLOAD_FOLDER, "*")):
        if os.path.isfile(filepath):
            stat = os.stat(filepath)
            files.append(
                {
                    "name": os.path.basename(filepath),
                    "size": stat.st_size,
                    "modified": datetime.fromtimestamp(stat.st_mtime).strftime(
                        "%Y-%m-%d %H:%M:%S"
                    ),
                    "download_url": f"/download/{os.path.basename(filepath)}",
                }
            )

    # 按修改时间倒序排列
    files.sort(key=lambda x: x["modified"], reverse=True)
    return files


@app.route("/files")
def list_files():
    """显示崩溃文件列表"""
    files = get_file_info()

    # 使用三重引号避免格式化问题，使用 % 格式化替代 {}
    html_template = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>崩溃文件列表</title>
        <style>
            body { font-family: Arial, sans-serif; margin: 20px; }
            table { border-collapse: collapse; width: 100%%; }
            th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
            th { background-color: #f2f2f2; }
            tr:nth-child(even) { background-color: #f9f9f9; }
        </style>
    </head>
    <body>
        <h1>崩溃文件列表</h1>
        <p>目录: %s</p>
        <table>
            <tr>
                <th>文件名</th>
                <th>大小</th>
                <th>修改时间</th>
                <th>操作</th>
            </tr>
            %s
        </table>
    </body>
    </html>
    """

    file_rows = ""
    for file_info in files:
        size_mb = file_info["size"] / (1024 * 1024)
        file_rows += """
            <tr>
                <td>%s</td>
                <td>%.2f MB</td>
                <td>%s</td>
                <td><a href="%s">下载</a></td>
            </tr>
        """ % (
            file_info["name"],
            size_mb,
            file_info["modified"],
            file_info["download_url"],
        )

    return html_template % (os.path.abspath(UPLOAD_FOLDER), file_rows)


@app.route("/download/<filename>")
def download_file(filename):
    """下载崩溃文件"""
    try:
        return send_from_directory(UPLOAD_FOLDER, filename, as_attachment=True)
    except FileNotFoundError:
        return jsonify({"error": "File not found"}), 404


@app.route("/health")
def health_check():
    """健康检查端点"""
    return jsonify(
        {
            "status": "healthy",
            "timestamp": datetime.now().isoformat(),
            "service": "Crashpad File Server",
        }
    )


@app.route("/")
def index():
    """首页"""
    return jsonify(
        {
            "message": "Crashpad File Server is running",
            "endpoints": {
                "upload": "/upload (POST) - 接收崩溃文件",
                "files": "/files (GET) - 查看文件列表",
                "health": "/health (GET) - 健康检查",
            },
        }
    )


if __name__ == "__main__":
    print("启动 Crashpad 文件服务器...")
    print(f"上传目录: {os.path.abspath(UPLOAD_FOLDER)}")
    print("端点:")
    print("  POST http://127.0.0.1:8000/upload - 接收崩溃文件")
    print("  GET  http://127.0.0.1:8000/files - 查看文件列表")
    print("  GET  http://127.0.0.1:8000/health - 健康检查")
    print("  GET  http://127.0.0.1:8000/ - 首页信息")

    app.run(host="127.0.0.1", port=8000, debug=False)
