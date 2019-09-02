from os import path


def data_dir(*args):
    """Return a path relative to the `data` directory"""
    return path.join(path.dirname(__file__), "..", "..", "data", *args)


def test_dir(*args):
    return data_dir("test", *args)
