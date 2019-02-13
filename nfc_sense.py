import binascii
import nfc
import nfc.tag.tt3
import hmac

def main():
    clf = nfc.ContactlessFrontend('usb')
    #212F = Felica
    target_req = nfc.clf.RemoteTarget("212F")
    #0003 = Suica
    #target_req.sensf_req = bytearray.fromhex("0000030000")
    while True:
        target_res = clf.sense(target_req, iterations=10, interval=0.01)
        if target_res != None:
            tag = nfc.tag.tt3.Type3Tag(clf, target_res)
            print hmac.new(b'scitech', tag.idm).hexdigest()
            #print binascii.hexlify(tag.idm)
            break
    clf.close()

if __name__ == '__main__':
    main()
