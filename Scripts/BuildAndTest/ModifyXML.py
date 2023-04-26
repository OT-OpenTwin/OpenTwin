import sys
import xml.etree.ElementTree as ET
tree = ET.parse(sys.argv[1])
root = tree.getroot()

for i in root.findall('testsuite'):
    prepend = i.get('name') + '_'
    name = i.get('name')
    i.set('name',sys.argv[2])

    for name in i:
        ele_name = name.get('name')
        new_name = prepend + ele_name
        name.set('name',new_name)

tree.write(sys.argv[3])
