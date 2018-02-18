Import('env')
from base64 import b64decode

#
# Dump build environment (for debug)
#print env.Dump()
#

env.Replace(UPLOADOTACMD='node ..\firmware-uploader\index.js --binary '+"$SOURCE"+ ' --host ' + "$UPLOAD_PORT")

# uncomment line below to see environment variables
#print ARGUMENTS
