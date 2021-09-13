#!/usr/bin/python3

import re
import logging
import subprocess
from pathlib import Path
from datetime import datetime, timedelta


SNAPSHOT_ROOT = Path('/.snapshots')
SNAPSHOT_FORMAT = '%Y-%m-%d'
STORAGE_PATH = Path('/var/run/media/kr2/KRR_STORAGE/backups')
AGE_PUBKEY = Path('/home/kr2/kririae.keys')
LOG_FORMAT = "%(asctime)s (%(levelname)s): %(message)s"
NKEEP = 2

# logger initialization
logging.basicConfig(filename='/home/kr2/Misc/btrfs-auto-backup.log',
                    level=logging.DEBUG, format=LOG_FORMAT)
logger = logging.getLogger(__name__)


def exe_cmd(cmd: str):
    logger.info('execute: %r', cmd)
    ps = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    output = ps.communicate()[0].decode('utf-8')
    logger.info('exe res: %r', output)


def ext_time(x):
    return datetime.strptime(x.split('_')[0], SNAPSHOT_FORMAT)


class Subvol:
    """Subvolume configuration

    Parameters:
        path(type: pathlib.Path): Subvol absolute path.
        suffix(type: str): Snapshot suffix, for example, 'root' in '2021-08-10_root'.
        snapshot_inter(type: int): Interval between two snapshots.
        send_inter(type: int): Interval between two snapshots on external storage.
    """

    def __init__(self, path: Path, suffix: str, snapshot_inter: int, send_inter: int):
        self.path = path
        self.suffix = suffix
        self.snapshot_inter = snapshot_inter
        self.send_inter = send_inter

    def is_snapshot(self, fname: str, host=True):
        """Check if a filename without path satisfy the format of snapshot"""
        regex = r"^\d+-\d+-\d+_(\w+)$" if host else r"^\d+-\d+-\d+_\w+(\.\w+){1,3}$"
        return bool(re.search(regex, fname)) and self.suffix in fname

    def __snapshot_list(self, dir: Path, host: bool):
        return [i.name for i in dir.iterdir()
                if self.is_snapshot(i.name, host=host)]

    def __outdate_snapshots(self, dir: Path, inter: int, host: bool):
        if not dir.exists():
            logger.warning('dir do not exists: %s', dir)
            return []
        ls = self.__snapshot_list(dir, host=host)
        ls.sort(key=ext_time)
        ls = filter(lambda x: datetime.now() - ext_time(x) >
                    timedelta(days=inter * NKEEP), ls)
        return list(ls)

    def outdate_snapshots_host(self):
        """Outdate snapshots on host, will return (-\infty, snapshot_inter * NKEEP]"""
        return self.__outdate_snapshots(SNAPSHOT_ROOT, self.snapshot_inter, host=True)

    def outdate_snapshots_device(self):
        """Outdate snapshots on device, will return (-\infty, send_inter * NKEEP]"""
        return self.__outdate_snapshots(STORAGE_PATH, self.send_inter, host=False)

    def should_take_snapshot(self) -> bool:
        ls = self.__snapshot_list(SNAPSHOT_ROOT, host=True)
        ls.sort(key=ext_time)
        if ls == None or len(ls) == 0:
            return True
        return datetime.now() - ext_time(ls[-1]) > timedelta(days=self.snapshot_inter)

    def should_send_snapshot(self):
        if not STORAGE_PATH.exists():
            return False
        ls = self.__snapshot_list(STORAGE_PATH, host=False)
        ls.sort(key=ext_time)
        if ls == None or len(ls) == 0:
            return True
        return datetime.now() - ext_time(ls[-1]) > timedelta(days=self.send_inter)

    def take_snapshot(self):
        """Take a snapshot there isn't a snapshot already"""
        current = datetime.now().strftime(SNAPSHOT_FORMAT)
        snapshot_path = SNAPSHOT_ROOT / f'{current}_{self.suffix}'
        logger.info('try to take snapshot: %r', snapshot_path)
        if snapshot_path.exists():
            logger.info('snapshot already exists: %r', snapshot_path)
            return
        exe_cmd(f'sudo btrfs subvol snapshot -r {self.path} {snapshot_path}')

        if snapshot_path.exists():
            logger.info('snapshot success: %r', snapshot_path)
        else:
            logger.warning('snapshot failed: %r', snapshot_path)

    def send_snapshots_to_device(self, zstd=True, age=True):
        """Send snapshot to device if {device exists, there's a snapshot today taken already}"""
        current = datetime.now().strftime(SNAPSHOT_FORMAT)
        snapshot_src = SNAPSHOT_ROOT / f'{current}_{self.suffix}'
        if not snapshot_src.exists():
            logger.warning(
                'snapshot src do not exists, cancel sending: %r', snapshot_src)
            return

        zstd_suffix = '.zst' if zstd else ''
        age_suffix = '.age' if age else ''
        snapshot_dst = STORAGE_PATH / \
            f'{current}_{self.suffix}.full{zstd_suffix}{age_suffix}'

        logger.info('try to send snapshot: %r', snapshot_dst)
        if snapshot_dst.exists():
            logger.info('snapshot already exists: %r', snapshot_dst)
            return

        cmd = f'sudo btrfs send {SNAPSHOT_ROOT / snapshot_src}'
        if zstd:
            cmd += ' | zstd -T0 -'
        if age:
            cmd += f' | age -R {AGE_PUBKEY} -'
        cmd += f' > {snapshot_dst}'
        exe_cmd(cmd)

        if snapshot_dst.exists():
            logger.info('send snapshot success: %r', snapshot_dst)
        else:
            logger.warning('send snapshot failed: %r', snapshot_dst)

    def remove_snapshots_host(self):
        if not SNAPSHOT_ROOT.exists():
            logger.warning(
                'snapshot src do not exists, cancel deleting: %r', SNAPSHOT_ROOT)
            return
        out_host = self.outdate_snapshots_host()
        for snapshot_name in out_host:
            path = SNAPSHOT_ROOT / snapshot_name
            if not path.exists():
                logger.warning(
                    'snapshot %r on host do not exists, cancel deleting', path)
            else:
                # destructive operation
                exe_cmd(f'sudo btrfs subvol del {path}')

    def remove_snapshots_device(self):
        if not STORAGE_PATH.exists():
            logger.info(
                'device do not exists, cancel deleting: %r', STORAGE_PATH)
            return
        out_dev = self.outdate_snapshots_device()
        for fname in out_dev:
            path = STORAGE_PATH / fname
            if not path.exists():
                logger.warning(
                    'snapshot %r on device do not exists, cancel deleting', path)
            else:
                # destructive operation
                exe_cmd(f'rm {path}')


# Path, suffix name, snapshot interval, send snapshot interval
BACKUP_LIST = [Subvol(Path('/'), 'root', 2, 5),
               Subvol(Path('/home'), 'home', 1, 3)]


def scheduler():
    """
    A function intended called by systemd timer.
    Non-destructive operation, no data will be deleted currently.
    Whenever it's called, it will invoke an examination on host and device.
    Note:
        The operation is in days, which means that if I invoked scheduler today already, 
        the second call in the same day will not perform anything on snapshots.

        For stability and safety, do not invoke the delete process currently
        instead, check log file for further information.
    """

    for i in BACKUP_LIST:
        if i.should_take_snapshot():
            i.take_snapshot()
        if i.should_send_snapshot():
            i.send_snapshots_to_device()

        out_host = i.outdate_snapshots_host()
        out_dev = i.outdate_snapshots_device()
        if len(out_host) >= 1:
            logger.info('should delete on host: %r', out_host)
        if len(out_dev) >= 1:
            logger.info('should delete on dev: %r', out_dev)
        i.remove_snapshots_host()
        i.remove_snapshots_device()


def main():
    scheduler()


if __name__ == '__main__':
    main()
