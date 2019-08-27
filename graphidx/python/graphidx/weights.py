class ConstantWeights:
    def __init__(self, c: float):
        self.c = c

    def __getitem__(self, _):
        return self.c

    def __call__(self, _):
        return self.c


class ArrayWeights:
    def __init__(self, a):
        self.a = a

    def __getitem__(self, i):
        return self.a[i]

    def __call__(self, i):
        return self.a[i]
