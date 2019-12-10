#!/usr/bin/env python
#coding=utf-8

from M2Crypto import X509, RSA, EVP
import hashlib
import base64
import sys
import tarfile
import tempfile
import os
import shutil

'''
1001, invalid package
1002, invalid appconfig.xml
1003, package not found
1004, icon file not exist
1005, appconfig.xml not exist in pkg
1006, invalid permission
'''

class AppError(Exception): pass
'''
invalid package Exception handle
'''
class InvalidPackage(AppError):
  def __init__(self, value):
    self.value = value
  def __str__(self):
    return repr(self.value)

'''
write hashes or signatures to a file
'''
def write_to_file(something2write, filename):
  target = open(filename, 'w')
  for key, value in something2write.items():
    target.write("Name:" + key)
    target.write("\n")
    target.write("SHA1-Digest:" + value)
    target.write("\n")
    target.write("\n")

  target.close()

'''
get base path
'''
def getbasepath(tarball):
  ret = ""
  try:
    x = tarfile.open(tarball, "r:gz")
    members = x.getmembers()
    ret = members[0].name

    x.close()
  except tarfile.ReadError as e:
    raise InvalidPackage("open tarball failed")

  return ret

'''
get file list from a directory
'''
def getfilelist(dir):
  filelist=[]

  dir = os.path.abspath(dir)
  for root, subdirs, files in os.walk(dir):
    for file in files:
       filelist.append(os.path.join(root, file))

  return filelist

'''
check pemfile password
'''
def checkPasswd(pemfile, password):
    def mypasswd(*args):
      return password

    try:
      key = EVP.load_key(pemfile, mypasswd)
    except Exception,ex:
#  except EVP.EVPError as e:
      return -1
    return 0

'''
hash file with its content (SHA1)
'''
def hashfile(filecontent):
  sha1 = hashlib.sha1()
  sha1.update(filecontent)
  rethash = base64.b64encode(sha1.digest())
  return rethash

'''
hash file with filename (SHA1)
'''
def hashFileWithName(fineName, block_size=64 * 1024):
  with open(fineName, 'rb') as f:
    sha1 = hashlib.sha1()
    while True:
      data = f.read(block_size)
      if not data:
        break
      sha1.update(data)
    rethash = base64.b64encode(sha1.digest())
    return rethash

'''
sign package
'''
def sign_package(srcpackage, destpackage, pemfile, passwd, keyfile):
  tmpdir = tempfile.mkdtemp()
  tar = tarfile.open(srcpackage)
  tar.extractall(tmpdir)
  tar.close()

  #add new signature
  #base = getbasepath(srcpackage)
  #dir2sign = os.path.join(tmpdir, base)
  dir2sign = tmpdir
  meta_inf = os.path.join(dir2sign, "META-INF")
  #os.mkdir(meta_inf)

  #remove DEBUG.KEY
  #debug_key = os.path.join(meta_inf, "DEBUG.KEY")
+  #remove SyberOSSdkDebug.pem
+  debug_key = os.path.join(meta_inf, "SyberOSSdkDebug.pem")

  if os.path.exists(debug_key):
    os.remove(debug_key)

  hashmap, signmap = sign_project(dir2sign, pemfile, passwd)
  write_to_file(hashmap, os.path.join(meta_inf, "MANIFEST.HF"))
  write_to_file(signmap, os.path.join(meta_inf, "CERT.SF"))

  #add *.crt file
  shutil.copy(keyfile, meta_inf)

  #tarfile
  newtar = tarfile.open(destpackage, "w:gz")
  for root, subdirs, files in os.walk(dir2sign):
    for file in files:
      fullpath = os.path.join(root, file)
      newtar.add(fullpath, arcname=fullpath[len(dir2sign) + 1:])

  newtar.close()

  shutil.rmtree(tmpdir, ignore_errors=True)

'''
sign project
'''
def sign_project(dir2sign, pemfile, passwd):
  hashmap = {}
  signmap = {}

  dir2sign = os.path.abspath(dir2sign)
  base = dir2sign

  #meta_inf = os.path.join(dir2sign, "META-INF")
  #if os.path.isdir(meta_inf):
  #  shutil.rmtree(meta_inf, ignore_errors=True)

  filelist = getfilelist(dir2sign)
  for key in filelist:
    if key != base + "/META-INF/MANIFEST.HF" and \
       key != base + "/META-INF/CERT.SF" and \
       key != base + "/META-INF/release.pem":
      k = key[len(base) + 1:]
      hashmap[k] = hashFileWithName(key)
      signmap[k] = sign_file(hashmap[k], pemfile, passwd)

  return hashmap, signmap

'''
signature a hash
'''
def sign_file(digest, pemfile, passwd):
  def mypasswd(*args):
    return passwd

  key = EVP.load_key(pemfile, mypasswd)
  key.reset_context(md="sha1")
  key.sign_init()
  key.sign_update(digest)
  signature = base64.b64encode(key.sign_final())

  return signature

