import struct
import time
import os


def generate_multi_arch_file(filename, arch_list):
    """
    生成包含多份 ArchData 的二进制文件
    :param filename: 输出文件名
    :param arch_list: 包含字典的列表，每个字典代表一个 ArchData
    """

    # ==========================
    # 1. 准备头部数据
    # ==========================

    # 0-7字节: unix时间戳 (int64)
    timestamp = int(time.time())

    # 8-9字节: 数量 (unsigned short)
    count = len(arch_list)

    # 检查数量是否超过 unsigned short 最大值 (65535)
    if count > 65535:
        raise ValueError("数据数量超过 unsigned short 最大值 (65535)")

    print(f"准备生成文件: {filename}")
    print(f"时间戳: {timestamp}")
    print(f"建筑数量: {count}")

    # ==========================
    # 2. 打包头部
    # ==========================
    # < : 小端序
    # q : long long (8 bytes) -> time_t
    # H : unsigned short (2 bytes) -> count
    header_data = struct.pack("<qH", timestamp, count)

    # ==========================
    # 3. 打包结构体数据体
    # ==========================
    # ArchData 结构:
    # no_(B), level_(B), x_(B), y_(B), current_hp_(I), current_capacity_(I)
    # 总共 1+1+1+1+4+4 = 12 字节
    struct_fmt = "<BBBBII"

    body_data = bytearray()

    for arch in arch_list:
        packed_item = struct.pack(struct_fmt, arch["no"], arch["level"], arch["x"], arch["y"], arch["current_hp"], arch["current_capacity"])
        body_data.extend(packed_item)

    # ==========================
    # 4. 写入文件
    # ==========================
    total_data = header_data + body_data

    with open(filename, "wb") as f:
        f.write(total_data)

    print(f"写入完成，文件总大小: {len(total_data)} 字节 (预期: {10 + count * 12})")
    print("-" * 30)


def verify_file(filename):
    """
    读取并解析文件，验证数据正确性
    """
    if not os.path.exists(filename):
        print("文件不存在")
        return

    print("开始验证文件读取...")

    with open(filename, "rb") as f:
        # 1. 读取头部 (10字节)
        header_bytes = f.read(10)
        if len(header_bytes) < 10:
            print("文件头损坏")
            return

        timestamp, count = struct.unpack("<qH", header_bytes)
        print(f"[Header] Time: {timestamp}, Count: {count}")

        # 2. 读取结构体列表
        struct_size = 12  # 1+1+1+1+4

        print("[Body Data]")
        for i in range(count):
            data = f.read(struct_size)
            if len(data) < struct_size:
                print(f"错误: 第 {i+1} 个结构体数据不完整")
                break

            # 解析
            # no, level, x, y, current_hp, current_capacity
            vals = struct.unpack("<BBBBII", data)
            print(f"  #{i+1}: No={vals[0]}, Level={vals[1]}, Pos=({vals[2]},{vals[3]}), HP={vals[4]}, Cap={vals[5]}")


def write_two_longlong(a: int, b: int, filename: str):
    # C++ long long = 8 字节 = struct 的 'q'
    # '<qq' 表示 little-endian、两个 long long
    data = struct.pack("<qq", a, b)

    with open(filename, "wb") as f:
        f.write(data)

    print(f"已写入文件: {filename}")


if __name__ == "__main__":
    output_file = "MainVillageData.dat"

    # hp在存储的数据文件里不重要，只是一个占位填充
    data_list = [
        {"no": 0, "level": 4, "x": 0, "y": 0, "current_hp": 0, "current_capacity": 0},
        {"no": 1, "level": 4, "x": 10, "y": 0, "current_hp": 0, "current_capacity": 0},
        {"no": 1, "level": 4, "x": 11, "y": 0, "current_hp": 0, "current_capacity": 0},
        {"no": 1, "level": 4, "x": 10, "y": 1, "current_hp": 0, "current_capacity": 0},
        {"no": 1, "level": 4, "x": 11, "y": 1, "current_hp": 0, "current_capacity": 0},
        {"no": 1, "level": 1, "x": 10, "y": 2, "current_hp": 0, "current_capacity": 0},
        {"no": 1, "level": 1, "x": 11, "y": 2, "current_hp": 0, "current_capacity": 0},
        {"no": 1, "level": 1, "x": 10, "y": 3, "current_hp": 0, "current_capacity": 0},
        {"no": 1, "level": 1, "x": 11, "y": 3, "current_hp": 0, "current_capacity": 0},
        {"no": 10, "level": 1, "x": 30, "y": 30, "current_hp": 0, "current_capacity": 10},
        {"no": 11, "level": 4, "x": 30, "y": 33, "current_hp": 0, "current_capacity": 10},
        {"no": 12, "level": 1, "x": 30, "y": 36, "current_hp": 0, "current_capacity": 10},
        {"no": 13, "level": 1, "x": 30, "y": 39, "current_hp": 0, "current_capacity": 10},
    ]

    # 生成
    generate_multi_arch_file(output_file, data_list)

    # 验证
    verify_file(output_file)

    # write_two_longlong(4000, 6000, "SourceData.dat")
