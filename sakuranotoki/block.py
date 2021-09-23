from typing import List, Dict


class Block:
    block_num: int = -1
    name = None
    scripts: Dict[str, str] = {}

    def __init__(self):
        pass

    def update_from_human_readable(self, block_s: List[str]):
        """update from human readable"""
        current_script = ''
        self.block_num = -1
        self.name = None  # reset name
        self.scripts = {}
        for line in block_s:
            if line.startswith('block'):
                assert self.block_num == -1
                self.block_num = int(line[6:])
            elif 'name' in line:
                assert self.name is None
                self.name = line[8:]
            elif 'script' in line:
                current_script = line[10:]
                assert current_script != ''
                self.scripts[current_script] = ''
            elif 'trans' in line:
                self.scripts[current_script] = line[9:]
                current_script = ''
            else:
                assert False
        assert self.block_num != -1
        assert len(self.scripts) > 0

    def update_from_para(self, block_num: int, scripts: List[str], name: str = None):
        """update from precise parameters"""
        self.block_num = block_num
        self.scripts = {key: '' for key in scripts}
        self.name = name

    def export_to_human_readable(self):
        result = [f'block: {self.block_num}']
        if self.name is not None:
            result.append(f'  name: {self.name}')
        for i, j in self.scripts.items():
            result.append(f'  script: {i}')
            result.append(f'  trans: {j}')
        return result

    def export_to_ast(self):
        result = []
        if self.name is not None:
            result.append('name = {"' + self.name + '"},')
        for src, dst in self.scripts.items():
            result.append(f'"{dst}",')
            result.append('{"rt2"},')
        return result


def block_test():
    _input = """block: 00012\r
  name: 静流\r
  script: 「そりゃまた古い常連さんですね。\r
  trans: qwq\r
  script: まぁ、座ってくださいよ」\r
  trans: qwq"""
    block = Block()
    print(_input.splitlines())
    block.update_from_human_readable(_input.splitlines())
    print(block.export_to_human_readable())
    print(block.export_to_ast())


if __name__ == '__main__':
    block_test()
