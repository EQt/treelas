from lxml import etree
import re


def remove_css(fname: str, out: str):
    xml = etree.parse(fname)
    r = xml.getroot()
    ns = r.nsmap.copy()
    [style] = xml.xpath('//svg:style', namespaces=dict(svg=ns[None]))
    classes = style.text.strip().split()
    cls = dict(map(lambda c: re.findall(r'\.(.+)\{(.+)\}', c)[0], classes))

    # modify svg
    r.remove(style)
    for e in xml.xpath('//svg:*[@class]', namespaces=dict(svg=ns[None])):
        c = e.attrib['class']
        e.attrib['style'] = cls[c]
        del e.attrib['class']
    with open(out, 'w') as io:
        xml.write(io.buffer)


if __name__ == '__main__':
    from os import path
    fname = path.join(path.dirname(__file__), '..', 'data', 'image', 'phantom.svg')
    out = fname.replace('phantom.svg', 'phantom.2.svg')
    remove_css(fname, out)
