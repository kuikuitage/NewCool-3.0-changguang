#ifndef _DES_H_
#define _DES_H_

#define DES_IP              1
#define DES_IP_1            2
#define DES_RIGHT           4
#define DES_HASH            8

#define DES_ECS2_DECRYPT    (DES_IP | DES_IP_1 | DES_RIGHT)
#define DES_ECS2_CRYPT      (DES_IP | DES_IP_1)

void des(unsigned char key[], unsigned char mode, unsigned char data[]);
void doPC(unsigned char data[]);

int IPTV_3DesEncrypt(char *key,char *data,char *out_data,int max_out_len);
int IPTV_3DesDecrypt(char *key,char *data,char *out_data,int max_out_len);

#endif



