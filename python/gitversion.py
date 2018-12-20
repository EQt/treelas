import re
import subprocess as sp


def describe_tag(default):
    try:
        ver = sp.check_output('git describe --tag'.split())
        ver = ver.decode().strip()
        m = re.match(r'v(?P<ver>\d+(\.\d+)*)((-(?P<dev>\d+))(-.*))?', ver)
        if m.group('dev') is not None:
            return f"{m.group('ver')}.dev{m.group('dev')}"
        return m.group('ver')
    except sp.CalledProcessError:
        return default
