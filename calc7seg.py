#!/usr/bin/python

def onoff_str(s, char_tuples, value, onchar, offchar) :
    for c, v in char_tuples :
        if v & value :
            s = s.replace(c,onchar)
        else :
            s = s.replace(c,offchar)
    return s


def draw_7seg(v) :
    print('')
    print('')
    print('')
    print('current value of control byte: 0x%02x'%(v))
    print('')
    print(          '     0      ')
    print(onoff_str('    %%%%    ',[('%', 0x01),           ], v,'#', '.'))
    print(onoff_str(' 5 %    $ 1 ',[('%', 0x20), ('$',0x02)], v,'#', ':'))
    print(onoff_str('   % 6  $   ',[('%', 0x20), ('$',0x02)], v,'#', ':'))
    print(onoff_str('    %%%%    ',[('%', 0x40),           ], v,'#', '.'))
    print(onoff_str(' 4 %    $ 2 ',[('%', 0x10), ('$',0x04)], v,'#', ':'))
    print(onoff_str('   %    $   ',[('%', 0x10), ('$',0x04)], v,'#', ':'))
    print(onoff_str('    %%%%    ',[('%', 0x08),           ], v,'#', '\''))
    print(          '     3      ')


if __name__ == '__main__' :
    v = 0
    while True :
        draw_7seg(v)
        s = input('Toggle Segments (q to quit)>')
        if 'q' in s :
            break
        for k in range(8) :
            if '%d'%(k) in s :
                v = v ^ (1<<k)


