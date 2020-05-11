class DeQue:
    """
    Double ended queue
    """
    def __init__(self, n=None):
        from collections import deque
        from collections.abc import Iterable
        self._e = deque(n) if isinstance(n, Iterable) else deque()

    def pop(self, forward: bool = True):
        return self._e.popleft() if forward else self._e.pop()

    def peek(self, forward: bool = True):
        return self._e[0 if forward else -1]

    def push(self, x, forward: bool = True):
        if forward:
            self._e.appendleft(x)
        else:
            self._e.append(x)

    def __bool__(self) -> bool:
        return bool(self._e)

    def __len__(self) -> len:
        return len(self._e)

    def __repr__(self) -> str:
        name = type(self).__name__
        amen = type(self._e).__name__
        r = repr(self._e)
        assert r.startswith(amen)
        return name + r[len(amen):]

    def __iter__(self):
        return iter(self._e)
