from __future__ import annotations


class Weights:
    @staticmethod
    def new(x) -> Weights:
        """
        Depending on the type of x, construct `ConstantWeights`
        or `ArrayWeights`
        """
        if isinstance(x, Weights):
            return x
        elif isinstance(x, (float, int)):
            return ConstantWeights(x)
        elif hasattr(x, '__getitem__'):
            return ArrayWeights(x)
        raise ValueError(type(x))


class ConstantWeights(Weights):
    def __init__(self, c: float):
        self.c = c

    def __getitem__(self, _):
        return self.c

    def __call__(self, _):
        return self.c


class ArrayWeights(Weights):
    def __init__(self, a):
        self.a = a

    def __getitem__(self, i):
        return self.a[i]

    def __call__(self, i):
        return self.a[i]
