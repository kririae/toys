# !/usr/bin/python3
# -*- coding: utf-8 -*-

# take *.script as a input
import time
import json
import random
import requests
from block import Block
from typing import List
from hashlib import md5
from functools import reduce

# configuration
appid = ''
appkey = ''

from_lang = 'jp'
to_lang = 'zh'

endpoint = 'http://api.fanyi.baidu.com'
path = '/api/trans/vip/translate'
url = endpoint + path


def make_md5(s, encoding='utf-8'):
    return md5(s.encode(encoding)).hexdigest()


def trans(query: str):
    if not isinstance(query, str):
        assert False

    def proc():
        salt = random.randint(32768, 65536)
        sign = make_md5(appid + query + str(salt) + appkey)

        headers = {'Content-Type': 'application/x-www-form-urlencoded'}
        payload = {'appid': appid, 'q': query, 'from': from_lang, 'to': to_lang, 'salt': salt, 'sign': sign}

        r = requests.post(url, params=payload, headers=headers)
        return r.json()

    result = None
    for i in range(3):
        try:
            result = proc()
        except Exception as e:
            print("Encountered an error, try again")
            time.sleep(0.1)  # considering api's QOS
            continue
        else:
            break
    if result is None:
        assert False

    time.sleep(0.1)  # considering api's QOS
    return result["trans_result"][0]['dst']


def init_blocks(lines: List[str]):
    to_trans: List[Block] = []
    pre_idx = 0
    for idx, line in enumerate(lines[1:]):
        if line.startswith('block'):
            idx += 1
            cur_block = Block()
            cur_block.update_from_human_readable(lines[pre_idx: idx])
            to_trans.append(cur_block)
            pre_idx = idx

    # deal with the last block
    cur_block = Block()
    cur_block.update_from_human_readable(lines[pre_idx: len(lines)])
    to_trans.append(cur_block)

    for idx, i in enumerate(to_trans):
        assert idx == i.block_num
    assert len(to_trans) == len([i for i in lines if i.startswith('block')])
    return to_trans


def trans_file(filename: str):
    # read file lines
    with open(filename, 'r') as f:
        lines = [i[:-1] for i in f.readlines()]
    assert lines is not None
    to_trans: List[Block] = init_blocks(lines)

    # preview the result
    # for i in to_trans[:20]:
    #     print(i.export_to_human_readable())

    with open(filename + '.out', 'w') as f:
        for i in to_trans:
            for src, dst in i.scripts.items():
                if dst == '':
                    _start = 1 if src.startswith('「') else 0
                    _end = -1 if src.endswith('」') else len(src)
                    rep_src = src[_start:_end]
                    result = trans(rep_src)
                    if _start != 0:
                        result = '「' + result
                    if _end != len(src):
                        result = result + '」'
                    i.scripts[src] = result
                print(f'translate: {src} -> {i.scripts[src]}')
            output = i.export_to_human_readable()
            f.write('\n'.join(output) + '\n')


def gen_ast(file_trans: str, file_ast: str):
    with open(file_trans, 'r') as f:
        trans_lines = [i[:-1] for i in f.readlines()]
    assert trans_lines is not None
    with open(file_ast, 'r') as f:
        ast_lines = [i[:-1] for i in f.readlines()]
    assert ast_lines is not None

    blocks = init_blocks(trans_lines)

    f = open(file_ast, 'w')
    f.write('astver = 2.0\nast = {\n')
    # mode: 0 -> output nothing
    #       1 -> output current line
    mode = 1
    block_id = -1
    stack_size = -1
    for idx, ast_line in enumerate(ast_lines[2:]):
        if ast_line.strip().startswith('block'):
            mode = 1
            block_id = int(ast_line.strip()[6:-4])
        assert block_id != -1

        if stack_size != -1:
            if '{' in ast_line:
                stack_size += 1
            if '}' in ast_line:
                stack_size -= 1
        if 'ja = {' in ast_line:
            mode = 0
            stack_size = 1
            f.write(ast_line + '\n')
            f.write('				{\n')
            tmp = ["    				" + i for i in blocks[block_id].export_to_ast()]
            f.write('\n'.join(tmp) + '\n')
        elif stack_size == 1:
            mode = 1
            stack_size = -1

        if mode == 0:
            pass
        elif mode == 1:
            f.write(ast_line + '\n')

    f.close()


if __name__ == '__main__':
    # gen_ast('01_01.script.out', '01_01.ast')
    # trans_file("01_02_3.script")
    # trans_file("01_02_4.script")
    # trans_file("01_02_5.script")
    # trans_file("01_03_1.script")
    # trans_file("01_03_2.script")
    # trans_file("01_03_3.script")
    # trans_file("01_03_4.script")
    # trans_file("01_03_5.script")
    # trans_file("01_04_1.script")
    # trans_file("01_04_2.script")
    # trans_file("01_04_3.script")
    # trans_file("01_04_4.script")
    # trans_file("01_04_5.script")
    # trans_file('01_05.script')
    file_names = """01_01.ast
01_02_1.ast
01_02_2.ast
01_02_3.ast
01_02_4.ast
01_02_5.ast
01_03_1.ast
01_03_2.ast
01_03_3.ast
01_03_4.ast
01_03_5.ast
01_04_1.ast
01_04_2.ast
01_04_3.ast
01_04_4.ast
01_04_5.ast
01_05.ast
"""
    file_names = file_names.splitlines()
    print(file_names)
    for i in file_names:
        gen_ast(i.split('.')[0] + '.script.out', i)
