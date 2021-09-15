# !/usr/bin/python3
# -*- coding: utf-8 -*-
import os
import re
import core
import struct
import hashlib
import argparse
from functools import reduce
from pathlib import Path


def decrypt(file: bytes, key: bytes) -> bytes:
    # CPP Optimized function
    return core.fast_decrypt(file, key)

    # key_length = len(key)
    # result = []
    # for idx, b in enumerate(file):
    #     result.append(b ^ key[idx % key_length])
    # return bytes(result)


# `ast` file parser
def find_block(block, ja_idx):
    for i in range(ja_idx, 0, -1):
        if 'block' in block[i]:
            return i


def parse_block(name: str, block):
    name = name.split('.')[0] + '.script'
    f = open(name, 'w')
    for idx, i in enumerate(block):
        if 'ja = {' not in i or '= {' not in i:
            continue
        block_str = block[find_block(block, idx)]
        regex = r"^block_(\d+)"
        matches = list(re.finditer(regex, block_str))[0]
        f.writelines(f'block: {matches.group(1)}\n')
        print(f'block: {matches.group(1)}')

        layer = 0
        buffer = ""
        while True:
            line = block[idx]
            if '{' in line:
                layer += 1
            if '}' in line:
                layer -= 1
            assert layer >= 0

            if 'name = {' in line:
                f.writelines(f'  name: {line[9:-3]}\n')
                print(f'  name: {line[9:-3]}')
            elif '{"ruby",' in line:
                buffer += line[15:-3]
            elif line.startswith('"'):
                if '{"ruby",' not in block[idx - 1]:
                    buffer += line[1:-2]
                # else:
                #     buffer += f'^({line[1:-2]})'
            elif line == '{"rt2"},':
                f.writelines(f"  script: {buffer}\n")
                f.writelines(f"  trans: \n")
                print(f"  script: {buffer}")
                buffer = ""
            idx += 1
            if layer == 0:
                break
    f.close()


def save_file(filename: str, content: bytes):
    extract_path = Path('.') / 'extract_fake'
    file_path = extract_path / filename
    file_dir = file_path.parent
    print(f'saving file to: {str(file_path)}')
    if not file_dir.exists():
        file_dir.mkdir(parents=True)
    with file_path.open('wb') as f:
        f.write(content)


def repacker(dirname: str):
    repack_dir = Path('.') / dirname  # change it yourself
    file_list = [x.relative_to(repack_dir) for x in repack_dir.glob('**/*') if x.is_file()]
    name_len = reduce(lambda x, y: x + len(str(y).replace('/', '\\').encode('utf-8')), file_list, 0)
    index_size = 4 + name_len + len(file_list) * 4 * 4

    f = open('output.pfs', 'wb+')
    f.write(b'pf')
    f.write(b'\x38')
    f.write(struct.pack('I', index_size))
    f.write(struct.pack('i', len(file_list)))

    offset = index_size + 7  # start of files
    for i in file_list:
        file_size = (repack_dir / i).stat().st_size
        print(f'writing index: {str(i)}')
        fake_file = str(i).replace('/', '\\').encode('utf-8')
        f.write(struct.pack('i', len(fake_file)))
        f.write(fake_file)
        f.write(b'\x00\x00\x00\x00')  # empty 4 bytes
        f.write(struct.pack('i', offset))
        f.write(struct.pack('I', file_size))
        offset += file_size

    f.seek(7, 0)
    index = f.read(index_size)
    f.seek(index_size + 7, 0)

    key = hashlib.sha1()
    key.update(index)
    for i in file_list:
        print(f'writing content: {str(i)}')
        f.write(decrypt((repack_dir / i).read_bytes(), key.digest()))
    print(f'key: {key.hexdigest()}')
    print(index_size)
    f.close()


def unpacker(filename: str):
    f = open(filename, "rb")
    pf = f.read(2)

    if pf != b'pf':
        print("not a pfs file")
        exit(-1)

    version = int(f.read(1))
    assert version == 8
    file_size = os.path.getsize(filename)
    print(f'file size: {file_size}')

    def read_uint() -> int:
        return struct.unpack("I", f.read(4))[0]

    def read_int() -> int:
        return struct.unpack("i", f.read(4))[0]

    def read_str(length: int) -> str:
        return struct.unpack(f"{length}s", f.read(length))[0]

    index_size = read_uint()
    count = read_int()
    _cur = f.tell()
    f.seek(7, 0)
    index = f.read(index_size)
    print(index_size)
    f.seek(_cur, 0)

    key = hashlib.sha1()
    key.update(index)

    f_order = open('order.txt', 'w')

    for i in range(count):
        path_len = read_int()
        path = f.read(path_len).decode('utf-8')
        name = path.split('\\')[-1]

        # convert to to unix path
        path = path.replace('\\', '/')

        f.seek(4, 1)  # from the current position
        offset = read_uint()
        size = read_uint()
        # print(f"entry: {name} {offset} {size}")

        _cur = f.tell()  # save current position

        # Traverse through the files
        # if 'script' in path and 'ast' in path:
        # print("------")
        # print(f"extracting {path}")
        # print(f"  offset: {offset}\n  size: {size}")

        # index to offset
        f.seek(offset, 0)
        # print(f'reading {offset} {size}')
        file_content = f.read(size)
        file_content = decrypt(file_content, key.digest())

        save_file(path, file_content)

        if 'ast' in name and '01' in name:
            block = file_content.decode('utf-8').split('\r\n')
            block = [i.replace('\t', '') for i in block]
            parse_block(name, block)

        # write line without new line
        # with open(name, 'wb') as out_f:
        # out_f.write(file_content.encode('utf-8'))

        f.seek(_cur, 0)  # restore current position
    f_order.close()
    f.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Tools for sakuranotoki.')
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--unpack', nargs='?', type=str, help='unpack file')
    group.add_argument('--pack', nargs='?', type=str, help='pack directory')
    args = parser.parse_args().__dict__
    if args['unpack'] is not None:
        unpacker(args['unpack'])
    elif args['pack'] is not None:
        repacker(args['pack'])
    else:
        parser.print_help()
