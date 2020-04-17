#!/usr/bin/python
# -*- coding:UTF-8 -*-

"""
shellTools

Author
    Letter(nevermindzzt@gmail.com)

Date
    2020-04-14

Copyright
    (c) Letter 2020
"""

import os
import sys
import re

FILE_PATTERN = r"^.*?\.(c|h)$"
SHELL_EXPORT_PATTERN = r"^\s*SHELL_EXPORT_(CMD|CMD_AGENCY|VAR|USER|KEY)\s*\(\s*\S+\s*,\s*(\S+)\s*,.*$"

def isCodeFile(name):
    return re.match(FILE_PATTERN, name) != None

def getCodeFiles(path):
    names = os.listdir(path)
    files = []
    for name in names:
        fullName = path + '\\' + name
        if os.path.isdir(fullName):
            files = files + getCodeFiles(fullName)
        else:
            if re.match(FILE_PATTERN, name) != None:
                files.append(fullName)
    return files

def getFileEcoding(name):
    encoding = ""
    try:
        with open(name, encoding="utf-8") as f:
            f.read()
            encoding = "utf-8"
    except:
        try:
            with open(name, encoding="gbk") as f:
                f.read()
                encoding = "gbk"
        except:
            encoding = "ascii"
    return encoding
    

if __name__ == "__main__":
    path = sys.argv[1]
    itemList = []
    codeLine = ""
    for codeFile in getCodeFiles(path):
        try:
            with open(codeFile, encoding=getFileEcoding(codeFile)) as f:
                lineNum = 0
                while True:
                    line = f.readline()
                    if line:
                        line = line.replace("\r", " ").replace("\n", " ")
                        codeLine = codeLine + line
                        lineNum = lineNum + 1
                        if ";" in codeLine or "}" in codeLine:
                            matcher = re.match(SHELL_EXPORT_PATTERN, codeLine)
                            if matcher != None:
                                item = (matcher.group(1), matcher.group(2), codeFile, lineNum)
                                itemList.append(item)
                            codeLine = ""
                    else:
                        codeLine = ""
                        break
        except Exception as e:
            print(e)
            pass

    for item in sorted(itemList, key=lambda x:(x[1])):
        print("shell export %s, %s in file %s(%d)"
             %(item[0], item[1], item[2], item[3]))
