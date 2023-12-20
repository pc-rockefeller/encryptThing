import ctypes
import base64

import asyncio

import util

BUFFER_SIZE = 2048

_libSo = None

_encrypt = None
_decrypt = None


class Entry(ctypes.Structure):
    _fields_ = [('data', ctypes.c_char_p),
                ('length', ctypes.c_long)]


def _getLib() -> ctypes.CDLL:
    global _libSo
    if not _libSo:
        _libSo = ctypes.CDLL('./encryptor/encryptor.so')
        _libSo.init()
    return _libSo

def encrypt(message : str) -> tuple[str]:
    print('pythino encripto') # TODO
    global _encrypt
    if not _encrypt:
        _encrypt = _getLib().encrypt
        _encrypt.argtypes = [Entry, ctypes.c_char_p, ctypes.c_long] # textEntry, output, outputBuffer
    messageBytes = message.encode('ascii')
    messageEntry = Entry(messageBytes, len(messageBytes))
    outCharPtr = ctypes.create_string_buffer(BUFFER_SIZE)
    _encrypt(messageEntry, outCharPtr, BUFFER_SIZE)
    out = util.parseRecollected(outCharPtr.raw)
    print(out) # TODO REMOVE
    if len(out) < 3:
        return ('error parsing parsing recollection, len(out) < 3', '', '')
    error = out[0].decode('ascii')
    encryptedMessage = base64.b64encode(out[1]).decode('ascii')
    key = base64.b64encode(out[2]).decode('ascii')
    return (error, encryptedMessage, key)

def decrypt(encryptedMessage : str, key : str) -> tuple[str]:
    global _decrypt
    if not _decrypt:
        _decrypt = _getLib().decrypt
        _decrypt.argtypes = [Entry, Entry, ctypes.c_char_p, ctypes.c_long] # encryptedTextEntry, keyEntry, output, outputBuffer
    encryptedMessageBytes = base64.b64decode(encryptedMessage)
    encryptedMessageEntry = Entry(encryptedMessageBytes, len(encryptedMessageBytes))
    keyBytes = base64.b64decode(key)
    keyEntry = Entry(keyBytes, len(keyBytes))
    print(encryptedMessageBytes, keyBytes) # TODO REMOVE
    outCharPtr = ctypes.create_string_buffer(BUFFER_SIZE)
    _decrypt(encryptedMessageEntry, keyEntry, outCharPtr, BUFFER_SIZE)
    out = util.parseRecollected(outCharPtr.raw)
    if len(out) < 2:
        return ('error parsing parsing recollection, len(out) < 2', '')
    error = out[0].decode('ascii')
    decryptedMessage = base64.b64encode(out[1]).decode('ascii')
    key = base64.b64encode(out[1]).decode('ascii')
    return (error, decryptedMessage)


_testFlask = None

def testFlask(message : str):
    global _testFlask
    if not _testFlask:
        _testFlask = _getLib().testFlask
        _testFlask.argtypes = [Entry]
    messageBytes = message.encode('ascii')
    messageEntry = Entry(messageBytes, len(messageBytes))
    _testFlask(messageEntry)