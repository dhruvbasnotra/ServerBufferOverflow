#define BUFFER_SIZE 1024

#define _TMP_FILE_ "tmp.sftp.file"
#define _LS_FILES_ "ls -l > tmp.sftp.file"

#define _TYPE_CMD_ 0
#define _TYPE_PUT_ 1
#define _TYPE_GET_ 2
#define _TYPE_END_ 3

#define PRINTSTACK

typedef struct {
	char type;
	char filename[32];
	char buf[BUFFER_SIZE];
} Packet, *Packet_Ptr;

