# -*- coding: utf-8 -*-
import os
import re
import struct
import hashlib
import json
import numpy as np
from pathlib import Path


def decrypt(file: bytes, key: bytes) -> bytes:
    key_length = len(key)
    result = []
    for idx, b in enumerate(file):
        result.append(b ^ key[idx % key_length])
    return bytes(result)


# ast parser
def find_block(block, ja_idx):
    for i in range(ja_idx, 0, -1):
        if 'block' in block[i]:
            return i


def parse_block(block):
    for idx, i in enumerate(block):
        if 'ja = {' in i and '= {' in i:
            block_str = block[find_block(block, idx)]
            regex = r"^block_(\d+)"
            matches = list(re.finditer(regex, block_str))[0]
            print(f'    block num: {matches.group(1)}')

            layer = 0
            while True:
                line = block[idx]
                if '{' in line:
                    layer += 1
                if '}' in line:
                    layer -= 1
                assert layer >= 0

                if 'name = {' in line:
                    print(f'      name: {line[9:-3]}')
                if line.startswith('"'):
                    print(f'      script: {line[1:-2]}')
                idx += 1
                if layer == 0:
                    break


def packer():
    pass


def save_file(filename: str, content: bytes):
    cur_dir = os.path.abspath(__file__)
    dest = os.path.dirname(cur_dir)
    EXTRACT_PATH = Path(dest) / 'extract'
    FILE_PATH = EXTRACT_PATH / filename
    FILE_DIR = FILE_PATH.parent
    print(f'saving file to: {str(FILE_PATH)}')
    if not FILE_DIR.exists():
        FILE_DIR.mkdir(parents=True)
    with FILE_PATH.open('wb') as f:
        f.write(content)


def main():
    filename = "root.pfs"
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
    f.seek(_cur, 0)

    key = hashlib.sha1()
    key.update(index)

    for i in range(count):
        path_len = read_int()
        path = read_str(path_len).decode('utf-8')
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
        print(f"extracting {path}")
        print(f"  offset: {offset}\n  size: {size}")

        # index to offset
        f.seek(offset, 0)
        print(f'reading {offset} {size}')
        file_content = f.read(size)
        file_content = decrypt(file_content, key.digest())

        save_file(path, file_content)

        # block = file_content.split('\r\n')
        # block = [i.replace('\t', '') for i in block]
        # parse_block(block)

        # write line without new line
        # with open(name, 'wb') as out_f:
        # out_f.write(file_content.encode('utf-8'))

        f.seek(_cur, 0)  # restore current position
    f.close()


if __name__ == '__main__':
    main()
