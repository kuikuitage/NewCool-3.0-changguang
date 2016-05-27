### Generated automatically from Makefile.org by Configure.

##
## Makefile for OpenSSL
##
#-include ../../../branch_src_sharedMemory_integration/Tools/MConfig/.config
#include ../../../include/MakeConfig
TOP_DIR = .$(shell pwd | sed 's/\(.*\)\/montage-tech\(.*\)/\2/g' | sed 's:/[^/]*:/..:g')
-include $(TOP_DIR)/makeConfig



ifeq ($(USE_UC_OS_OS), Y)
-include $(TOP_DIR)/common.mk
CRYPTO_OBJS :=\
./crypto/aes/aes_cbc.o\
./crypto/aes/aes_cfb.o\
./crypto/aes/aes_core.o\
./crypto/aes/aes_ctr.o\
./crypto/aes/aes_ecb.o\
./crypto/aes/aes_ige.o\
./crypto/aes/aes_misc.o\
./crypto/aes/aes_ofb.o\
./crypto/asn1/a_bitstr.o\
./crypto/asn1/a_bool.o\
./crypto/asn1/a_bytes.o\
./crypto/asn1/a_d2i_fp.o\
./crypto/asn1/a_digest.o\
./crypto/asn1/a_dup.o\
./crypto/asn1/a_enum.o\
./crypto/asn1/a_gentm.o\
./crypto/asn1/a_hdr.o\
./crypto/asn1/a_i2d_fp.o\
./crypto/asn1/a_int.o\
./crypto/asn1/a_mbstr.o\
./crypto/asn1/a_meth.o\
./crypto/asn1/a_object.o\
./crypto/asn1/a_octet.o\
./crypto/asn1/a_print.o\
./crypto/asn1/a_set.o\
./crypto/asn1/a_sign.o\
./crypto/asn1/a_strex.o\
./crypto/asn1/a_strnid.o\
./crypto/asn1/a_time.o\
./crypto/asn1/a_type.o\
./crypto/asn1/a_utctm.o\
./crypto/asn1/a_utf8.o\
./crypto/asn1/a_verify.o\
./crypto/asn1/asn1_err.o\
./crypto/asn1/asn1_gen.o\
./crypto/asn1/asn1_lib.o\
./crypto/asn1/asn1_par.o\
./crypto/asn1/asn_moid.o\
./crypto/asn1/asn_pack.o\
./crypto/asn1/d2i_pr.o\
./crypto/asn1/d2i_pu.o\
./crypto/asn1/evp_asn1.o\
./crypto/asn1/f_enum.o\
./crypto/asn1/f_int.o\
./crypto/asn1/f_string.o\
./crypto/asn1/i2d_pr.o\
./crypto/asn1/i2d_pu.o\
./crypto/asn1/n_pkey.o\
./crypto/asn1/nsseq.o\
./crypto/asn1/p5_pbe.o\
./crypto/asn1/p5_pbev2.o\
./crypto/asn1/p8_pkey.o\
./crypto/asn1/t_bitst.o\
./crypto/asn1/t_crl.o\
./crypto/asn1/t_pkey.o\
./crypto/asn1/t_req.o\
./crypto/asn1/t_spki.o\
./crypto/asn1/t_x509.o\
./crypto/asn1/t_x509a.o\
./crypto/asn1/tasn_dec.o\
./crypto/asn1/tasn_enc.o\
./crypto/asn1/tasn_fre.o\
./crypto/asn1/tasn_new.o\
./crypto/asn1/tasn_typ.o\
./crypto/asn1/tasn_utl.o\
./crypto/asn1/x_algor.o\
./crypto/asn1/x_attrib.o\
./crypto/asn1/x_bignum.o\
./crypto/asn1/x_crl.o\
./crypto/asn1/x_exten.o\
./crypto/asn1/x_info.o\
./crypto/asn1/x_long.o\
./crypto/asn1/x_name.o\
./crypto/asn1/x_pkey.o\
./crypto/asn1/x_pubkey.o\
./crypto/asn1/x_req.o\
./crypto/asn1/x_sig.o\
./crypto/asn1/x_spki.o\
./crypto/asn1/x_val.o\
./crypto/asn1/x_x509.o\
./crypto/asn1/x_x509a.o\
./crypto/bf/bf_cfb64.o\
./crypto/bf/bf_ecb.o\
./crypto/bf/bf_enc.o\
./crypto/bf/bf_ofb64.o\
./crypto/bf/bf_skey.o\
./crypto/bio/b_dump.o\
./crypto/bio/b_print.o\
./crypto/bio/b_sock.o\
./crypto/bio/bf_buff.o\
./crypto/bio/bf_lbuf.o\
./crypto/bio/bf_nbio.o\
./crypto/bio/bf_null.o\
./crypto/bio/bio_cb.o\
./crypto/bio/bio_err.o\
./crypto/bio/bio_lib.o\
./crypto/bio/bss_acpt.o\
./crypto/bio/bss_bio.o\
./crypto/bio/bss_conn.o\
./crypto/bio/bss_dgram.o\
./crypto/bio/bss_fd.o\
./crypto/bio/bss_file.o\
./crypto/bio/bss_log.o\
./crypto/bio/bss_mem.o\
./crypto/bio/bss_null.o\
./crypto/bio/bss_sock.o\
./crypto/bn/bn_add.o\
./crypto/bn/bn_asm.o\
./crypto/bn/bn_blind.o\
./crypto/bn/bn_const.o\
./crypto/bn/bn_ctx.o\
./crypto/bn/bn_depr.o\
./crypto/bn/bn_div.o\
./crypto/bn/bn_err.o\
./crypto/bn/bn_exp.o\
./crypto/bn/bn_exp2.o\
./crypto/bn/bn_gcd.o\
./crypto/bn/bn_gf2m.o\
./crypto/bn/bn_kron.o\
./crypto/bn/bn_lib.o\
./crypto/bn/bn_mod.o\
./crypto/bn/bn_mont.o\
./crypto/bn/bn_mpi.o\
./crypto/bn/bn_mul.o\
./crypto/bn/bn_nist.o\
./crypto/bn/bn_prime.o\
./crypto/bn/bn_print.o\
./crypto/bn/bn_rand.o\
./crypto/bn/bn_recp.o\
./crypto/bn/bn_shift.o\
./crypto/bn/bn_sqr.o\
./crypto/bn/bn_sqrt.o\
./crypto/bn/bn_word.o\
./crypto/buffer/buf_err.o\
./crypto/buffer/buffer.o\
./crypto/cast/c_cfb64.o\
./crypto/cast/c_ecb.o\
./crypto/cast/c_enc.o\
./crypto/cast/c_ofb64.o\
./crypto/cast/c_skey.o\
./crypto/comp/c_rle.o\
./crypto/conf/conf_api.o\
./crypto/conf/conf_def.o\
./crypto/conf/conf_err.o\
./crypto/conf/conf_lib.o\
./crypto/conf/conf_mall.o\
./crypto/conf/conf_mod.o\
./crypto/conf/conf_sap.o\
./crypto/des/cbc3_enc.o\
./crypto/des/cbc_cksm.o\
./crypto/des/cbc_enc.o\
./crypto/des/cfb64ede.o\
./crypto/des/cfb64enc.o\
./crypto/des/cfb_enc.o\
./crypto/des/des_enc.o\
./crypto/des/des_old.o\
./crypto/des/des_old2.o\
./crypto/des/ecb3_enc.o\
./crypto/des/ecb_enc.o\
./crypto/des/ede_cbcm_enc.o\
./crypto/des/enc_read.o\
./crypto/des/enc_writ.o\
./crypto/des/fcrypt.o\
./crypto/des/fcrypt_b.o\
./crypto/des/ofb64ede.o\
./crypto/des/ofb64enc.o\
./crypto/des/ofb_enc.o\
./crypto/des/pcbc_enc.o\
./crypto/des/qud_cksm.o\
./crypto/des/rand_key.o\
./crypto/des/read2pwd.o\
./crypto/des/rpc_enc.o\
./crypto/des/set_key.o\
./crypto/des/str2key.o\
./crypto/des/xcbc_enc.o\
./crypto/dh/dh_asn1.o\
./crypto/dh/dh_check.o\
./crypto/dh/dh_depr.o\
./crypto/dh/dh_err.o\
./crypto/dh/dh_gen.o\
./crypto/dh/dh_key.o\
./crypto/dh/dh_lib.o\
./crypto/dsa/dsa_asn1.o\
./crypto/dsa/dsa_depr.o\
./crypto/dsa/dsa_err.o\
./crypto/dsa/dsa_gen.o\
./crypto/dsa/dsa_key.o\
./crypto/dsa/dsa_lib.o\
./crypto/dsa/dsa_ossl.o\
./crypto/dsa/dsa_sign.o\
./crypto/dsa/dsa_vrf.o\
./crypto/dso/dso_dl.o\
./crypto/dso/dso_dlfcn.o\
./crypto/dso/dso_err.o\
./crypto/dso/dso_lib.o\
./crypto/dso/dso_null.o\
./crypto/dso/dso_openssl.o\
./crypto/dso/dso_vms.o\
./crypto/dso/dso_win32.o\
./crypto/ec/ec2_mult.o\
./crypto/ec/ec2_smpl.o\
./crypto/ec/ec_asn1.o\
./crypto/ec/ec_check.o\
./crypto/ec/ec_curve.o\
./crypto/ec/ec_cvt.o\
./crypto/ec/ec_err.o\
./crypto/ec/ec_key.o\
./crypto/ec/ec_lib.o\
./crypto/ec/ec_mult.o\
./crypto/ec/ec_print.o\
./crypto/ec/ecp_mont.o\
./crypto/ec/ecp_nist.o\
./crypto/ec/ecp_smpl.o\
./crypto/ecdh/ech_err.o\
./crypto/ecdh/ech_key.o\
./crypto/ecdh/ech_lib.o\
./crypto/ecdh/ech_ossl.o\
./crypto/ecdsa/ecs_asn1.o\
./crypto/ecdsa/ecs_err.o\
./crypto/ecdsa/ecs_lib.o\
./crypto/ecdsa/ecs_ossl.o\
./crypto/ecdsa/ecs_sign.o\
./crypto/ecdsa/ecs_vrf.o\
./crypto/engine/eng_all.o\
./crypto/engine/eng_cnf.o\
./crypto/engine/eng_cryptodev.o\
./crypto/engine/eng_ctrl.o\
./crypto/engine/eng_dyn.o\
./crypto/engine/eng_err.o\
./crypto/engine/eng_fat.o\
./crypto/engine/eng_init.o\
./crypto/engine/eng_lib.o\
./crypto/engine/eng_list.o\
./crypto/engine/eng_openssl.o\
./crypto/engine/eng_padlock.o\
./crypto/engine/eng_pkey.o\
./crypto/engine/eng_table.o\
./crypto/engine/tb_cipher.o\
./crypto/engine/tb_dh.o\
./crypto/engine/tb_digest.o\
./crypto/engine/tb_dsa.o\
./crypto/engine/tb_ecdh.o\
./crypto/engine/tb_ecdsa.o\
./crypto/engine/tb_rand.o\
./crypto/engine/tb_rsa.o\
./crypto/engine/tb_store.o\
./crypto/err/err_all.o\
./crypto/err/err_openssl.o\
./crypto/err/err_prn.o\
./crypto/evp/bio_b64.o\
./crypto/evp/bio_enc.o\
./crypto/evp/bio_md.o\
./crypto/evp/bio_ok.o\
./crypto/evp/c_all.o\
./crypto/evp/c_allc.o\
./crypto/evp/c_alld.o\
./crypto/evp/digest.o\
./crypto/evp/e_aes.o\
./crypto/evp/e_bf.o\
./crypto/evp/e_camellia.o\
./crypto/evp/e_cast.o\
./crypto/evp/e_des.o\
./crypto/evp/e_des3.o\
./crypto/evp/e_idea.o\
./crypto/evp/e_null.o\
./crypto/evp/e_old.o\
./crypto/evp/e_rc2.o\
./crypto/evp/e_rc4.o\
./crypto/evp/e_rc5.o\
./crypto/evp/e_seed.o\
./crypto/evp/e_xcbc_d.o\
./crypto/evp/encode.o\
./crypto/evp/evp_acnf.o\
./crypto/evp/evp_enc.o\
./crypto/evp/evp_err.o\
./crypto/evp/evp_key.o\
./crypto/evp/evp_lib.o\
./crypto/evp/evp_pbe.o\
./crypto/evp/evp_pkey.o\
./crypto/evp/m_dss.o\
./crypto/evp/m_dss1.o\
./crypto/evp/m_ecdsa.o\
./crypto/evp/m_md2.o\
./crypto/evp/m_md4.o\
./crypto/evp/m_md5.o\
./crypto/evp/m_mdc2.o\
./crypto/evp/m_null.o\
./crypto/evp/m_ripemd.o\
./crypto/evp/m_sha.o\
./crypto/evp/m_sha1.o\
./crypto/evp/names.o\
./crypto/evp/openbsd_hw.o\
./crypto/evp/p5_crpt.o\
./crypto/evp/p5_crpt2.o\
./crypto/evp/p_dec.o\
./crypto/evp/p_enc.o\
./crypto/evp/p_lib.o\
./crypto/evp/p_open.o\
./crypto/evp/p_seal.o\
./crypto/evp/p_sign.o\
./crypto/evp/p_verify.o\
./crypto/hmac/hmac.o\
./crypto/idea/i_cbc.o\
./crypto/idea/i_cfb64.o\
./crypto/idea/i_ecb.o\
./crypto/idea/i_ofb64.o\
./crypto/idea/i_skey.o\
./crypto/lhash/lh_stats.o\
./crypto/lhash/lhash.o\
./crypto/md2/md2_dgst.o\
./crypto/md2/md2_one.o\
./crypto/md4/md4_dgst.o\
./crypto/md4/md4_one.o\
./crypto/md5/md5_dgst.o\
./crypto/md5/md5_one.o\
./crypto/objects/o_names.o\
./crypto/objects/obj_dat.o\
./crypto/objects/obj_err.o\
./crypto/objects/obj_lib.o\
./crypto/pem/pem_all.o\
./crypto/pem/pem_err.o\
./crypto/pem/pem_info.o\
./crypto/pem/pem_lib.o\
./crypto/pem/pem_oth.o\
./crypto/pem/pem_pk8.o\
./crypto/pem/pem_pkey.o\
./crypto/pem/pem_seal.o\
./crypto/pem/pem_sign.o\
./crypto/pem/pem_x509.o\
./crypto/pem/pem_xaux.o\
./crypto/pkcs7/example.o\
./crypto/pkcs7/pk7_asn1.o\
./crypto/pkcs7/pk7_attr.o\
./crypto/pkcs7/pk7_dgst.o\
./crypto/pkcs7/pk7_doit.o\
./crypto/pkcs7/pk7_lib.o\
./crypto/pkcs7/pk7_mime.o\
./crypto/pkcs7/pk7_smime.o\
./crypto/pkcs7/pkcs7err.o\
./crypto/rand/md_rand.o\
./crypto/rand/rand_egd.o\
./crypto/rand/rand_err.o\
./crypto/rand/rand_lib.o\
./crypto/rand/rand_nw.o\
./crypto/rand/rand_os2.o\
./crypto/rand/rand_unix.o\
./crypto/rand/rand_vms.o\
./crypto/rand/rand_win.o\
./crypto/rand/randfile.o\
./crypto/rc2/rc2_cbc.o\
./crypto/rc2/rc2_ecb.o\
./crypto/rc2/rc2_skey.o\
./crypto/rc2/rc2cfb64.o\
./crypto/rc2/rc2ofb64.o\
./crypto/rc4/rc4_enc.o\
./crypto/rc4/rc4_skey.o\
./crypto/ripemd/rmd_dgst.o\
./crypto/ripemd/rmd_one.o\
./crypto/rsa/rsa_asn1.o\
./crypto/rsa/rsa_chk.o\
./crypto/rsa/rsa_depr.o\
./crypto/rsa/rsa_eay.o\
./crypto/rsa/rsa_err.o\
./crypto/rsa/rsa_gen.o\
./crypto/rsa/rsa_lib.o\
./crypto/rsa/rsa_none.o\
./crypto/rsa/rsa_null.o\
./crypto/rsa/rsa_oaep.o\
./crypto/rsa/rsa_pk1.o\
./crypto/rsa/rsa_pss.o\
./crypto/rsa/rsa_saos.o\
./crypto/rsa/rsa_sign.o\
./crypto/rsa/rsa_ssl.o\
./crypto/rsa/rsa_x931.o\
./crypto/sha/sha1_one.o\
./crypto/sha/sha1dgst.o\
./crypto/sha/sha256_openssl.o\
./crypto/sha/sha512.o\
./crypto/sha/sha_dgst.o\
./crypto/sha/sha_one.o\
./crypto/stack/stack.o\
./crypto/txt_db/txt_db.o\
./crypto/x509/by_dir.o\
./crypto/x509/by_file.o\
./crypto/x509/x509_att.o\
./crypto/x509/x509_cmp.o\
./crypto/x509/x509_d2.o\
./crypto/x509/x509_def.o\
./crypto/x509/x509_err.o\
./crypto/x509/x509_ext.o\
./crypto/x509/x509_lu.o\
./crypto/x509/x509_obj.o\
./crypto/x509/x509_r2x.o\
./crypto/x509/x509_req.o\
./crypto/x509/x509_set.o\
./crypto/x509/x509_trs.o\
./crypto/x509/x509_txt.o\
./crypto/x509/x509_v3.o\
./crypto/x509/x509_vfy.o\
./crypto/x509/x509_vpm.o\
./crypto/x509/x509cset.o\
./crypto/x509/x509name.o\
./crypto/x509/x509rset.o\
./crypto/x509/x509spki.o\
./crypto/x509/x509type.o\
./crypto/x509/x_all.o\
./crypto/x509v3/pcy_cache.o\
./crypto/x509v3/pcy_data.o\
./crypto/x509v3/pcy_lib.o\
./crypto/x509v3/pcy_map.o\
./crypto/x509v3/pcy_node.o\
./crypto/x509v3/pcy_tree.o\
./crypto/x509v3/v3_addr.o\
./crypto/x509v3/v3_akey.o\
./crypto/x509v3/v3_akeya.o\
./crypto/x509v3/v3_alt.o\
./crypto/x509v3/v3_asid.o\
./crypto/x509v3/v3_bcons.o\
./crypto/x509v3/v3_bitst.o\
./crypto/x509v3/v3_conf.o\
./crypto/x509v3/v3_cpols.o\
./crypto/x509v3/v3_crld.o\
./crypto/x509v3/v3_enum.o\
./crypto/x509v3/v3_extku.o\
./crypto/x509v3/v3_genn.o\
./crypto/x509v3/v3_ia5.o\
./crypto/x509v3/v3_info.o\
./crypto/x509v3/v3_int.o\
./crypto/x509v3/v3_lib.o\
./crypto/x509v3/v3_ncons.o\
./crypto/x509v3/v3_ocsp.o\
./crypto/x509v3/v3_pci.o\
./crypto/x509v3/v3_pcia.o\
./crypto/x509v3/v3_pcons.o\
./crypto/x509v3/v3_pku.o\
./crypto/x509v3/v3_pmaps.o\
./crypto/x509v3/v3_prn.o\
./crypto/x509v3/v3_purp.o\
./crypto/x509v3/v3_skey.o\
./crypto/x509v3/v3_sxnet.o\
./crypto/x509v3/v3_utl.o\
./crypto/x509v3/v3err.o\





SSL_OBJS :=\
./crypto/comp/c_rle.o\
./crypto/comp/c_zlib.o\
./crypto/comp/comp_err.o\
./crypto/comp/comp_lib.o\
./crypto/cpt_err.o\
./crypto/cryptlib.o\
./crypto/ebcdic.o\
./crypto/ex_data.o\
./crypto/krb5/krb5_asn.o\
./crypto/mem_clr.o\
./crypto/mem_dbg.o\
./crypto/mem_openssl.o\
./crypto/o_str.o\
./crypto/o_time.o\
./crypto/ocsp/ocsp_asn.o\
./crypto/ocsp/ocsp_cl.o\
./crypto/ocsp/ocsp_err.o\
./crypto/ocsp/ocsp_ext.o\
./crypto/ocsp/ocsp_ht.o\
./crypto/ocsp/ocsp_lib.o\
./crypto/ocsp/ocsp_prn.o\
./crypto/ocsp/ocsp_srv.o\
./crypto/ocsp/ocsp_vfy.o\
./crypto/pkcs12/p12_add.o\
./crypto/pkcs12/p12_asn.o\
./crypto/pkcs12/p12_attr.o\
./crypto/pkcs12/p12_crpt.o\
./crypto/pkcs12/p12_crt.o\
./crypto/pkcs12/p12_decr.o\
./crypto/pkcs12/p12_init.o\
./crypto/pkcs12/p12_key.o\
./crypto/pkcs12/p12_kiss.o\
./crypto/pkcs12/p12_mutl.o\
./crypto/pkcs12/p12_npas.o\
./crypto/pkcs12/p12_p8d.o\
./crypto/pkcs12/p12_p8e.o\
./crypto/pkcs12/p12_utl.o\
./crypto/pkcs12/pk12err.o\
./crypto/pqueue/pqueue.o\
./crypto/store/str_err.o\
./crypto/store/str_lib.o\
./crypto/store/str_mem.o\
./crypto/store/str_meth.o\
./crypto/tmdiff.o\
./crypto/ui/ui_compat.o\
./crypto/ui/ui_err.o\
./crypto/ui/ui_lib.o\
./crypto/ui/ui_openssl.o\
./crypto/ui/ui_util.o\
./crypto/uid.o\
./ssl/bio_ssl.o\
./ssl/d1_both.o\
./ssl/d1_clnt.o\
./ssl/d1_enc.o\
./ssl/d1_lib.o\
./ssl/d1_meth.o\
./ssl/d1_pkt.o\
./ssl/d1_srvr.o\
./ssl/kssl.o\
./ssl/s23_clnt.o\
./ssl/s23_lib.o\
./ssl/s23_meth.o\
./ssl/s23_pkt.o\
./ssl/s23_srvr.o\
./ssl/s2_clnt.o\
./ssl/s2_enc.o\
./ssl/s2_lib.o\
./ssl/s2_meth.o\
./ssl/s2_pkt.o\
./ssl/s2_srvr.o\
./ssl/s3_both.o\
./ssl/s3_clnt.o\
./ssl/s3_enc.o\
./ssl/s3_lib.o\
./ssl/s3_meth.o\
./ssl/s3_pkt.o\
./ssl/s3_srvr.o\
./ssl/ssl_algs.o\
./ssl/ssl_asn1.o\
./ssl/ssl_cert.o\
./ssl/ssl_ciph.o\
./ssl/ssl_err.o\
./ssl/ssl_err2.o\
./ssl/ssl_lib.o\
./ssl/ssl_rsa.o\
./ssl/ssl_sess.o\
./ssl/ssl_stat.o\
./ssl/ssl_txt.o\
./ssl/t1_clnt.o\
./ssl/t1_enc.o\
./ssl/t1_lib.o\
./ssl/t1_meth.o\
./ssl/t1_srvr.o\

INCLUDES += -I$(TOP_DIR)/src/drv/ethernet -I$(TOP_DIR)/src/kware/network/openssl-0.9.8 -I$(TOP_DIR)/src/kware/network/openssl-0.9.8/crypto
			  
					  

CRYPTO_LIB_NAME                :=$(KWARE_LIBDIR)/libopenssla.a
SSL_LIB_NAME                :=$(KWARE_LIBDIR)/libopensslb.a

SUBDIRS =    

SSLCFLAGS ?=  -EL -march=mips32r2 \
               -mxgot \
               -mlong-calls  \
               -mabi=32 \
               -mno-abicalls \
               -Wall \
               -Wno-pointer-sign  \
               -Wno-pointer-to-int-cast \
               -ffunction-sections  \
				-O2	\
				-MMD	\
				-MP	\
				-MF"$object.d"	\
				-MT"$object.d"	\
				$(INC) $(MACRODEF) $(OPTIMIZE_DEFINES) $(DEBUG_DEFINES)  \
             \


all:  clear_lib  $(CRYPTO_OBJS) $(SSL_OBJS)
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i; done
	$(SHOW_COMMAND)$(AR) crs  $(CRYPTO_LIB_NAME)  $(CRYPTO_OBJS)
	$(SHOW_COMMAND)$(AR) crs  $(SSL_LIB_NAME)  $(SSL_OBJS)
	$(SHOW_COMMAND)$(RANLIB) $(CRYPTO_LIB_NAME)
	$(SHOW_COMMAND)$(RANLIB) $(SSL_LIB_NAME)


%.o: %.c
	$(SHOW_COMMAND)$(CC) $(SSLCFLAGS) $(INCLUDES) -c $*.c -o $@

%.o: %.cpp
	$(SHOW_COMMAND)$(CXX) $(SSLCFLAGS) $(INCLUDES) -c $*.cpp -o $@



clear_lib:

	$(SHOW_COMMAND)$(RM) -fr  $(CRYPTO_LIB_NAME)
  


##############################
# Clean up.
##############################
clean:



	$(SHOW_COMMAND)$(RM) -fr  $(LIB_NAME)
	find ./ -name "*.o" | xargs rm -fr
	find ./ -name "*.bbg" | xargs rm -fr
	find ./ -name "*.da" | xargs rm -fr
	find ./ -name "*.info" | xargs rm -fr
	find ./ -name "*.gcov" | xargs rm -fr
	find ./ -name "*.d" | xargs rm -fr

	

else
TOPDIR = .$(shell pwd | sed 's/\(.*\)\/montage-tech\(.*\)/\2/g' | sed 's:/[^/]*:/..:g')

-include $(TOPDIR)/common.mk

VERSION=0.9.8g
MAJOR=0
MINOR=9.8
SHLIB_VERSION_NUMBER=0.9.8
SHLIB_VERSION_HISTORY=
SHLIB_MAJOR=0
SHLIB_MINOR=9.8
SHLIB_EXT=
PLATFORM=dist
OPTIONS= no-camellia no-gmp no-krb5 no-mdc2 no-rc5 no-rfc3779 no-seed no-shared no-tlsext no-zlib no-zlib-dynamic
CONFIGURE_ARGS=dist
SHLIB_TARGET=

# HERE indicates where this Makefile lives.  This can be used to indicate
# where sub-Makefiles are expected to be.  Currently has very limited usage,
# and should probably not be bothered with at all.
HERE=.

# INSTALL_PREFIX is for package builders so that they can configure
# for, say, /usr/ and yet have everything installed to /tmp/somedir/usr/.
# Normally it is left empty.
INSTALL_PREFIX=
INSTALLTOP=/usr/local/ssl

# Do not edit this manually. Use Configure --openssldir=DIR do change this!
OPENSSLDIR=/usr/local/ssl

# NO_IDEA - Define to build without the IDEA algorithm
# NO_RC4  - Define to build without the RC4 algorithm
# NO_RC2  - Define to build without the RC2 algorithm
# THREADS - Define when building with threads, you will probably also need any
#           system defines as well, i.e. _REENTERANT for Solaris 2.[34]
# TERMIO  - Define the termio terminal subsystem, needed if sgtty is missing.
# TERMIOS - Define the termios terminal subsystem, Silicon Graphics.
# LONGCRYPT - Define to use HPUX 10.x's long password modification to crypt(3).
# DEVRANDOM - Give this the value of the 'random device' if your OS supports
#           one.  32 bytes will be read from this when the random
#           number generator is initalised.
# SSL_FORBID_ENULL - define if you want the server to be not able to use the
#           NULL encryption ciphers.
#
# LOCK_DEBUG - turns on lots of lock debug output :-)
# REF_CHECK - turn on some xyz_free() assertions.
# REF_PRINT - prints some stuff on structure free.
# CRYPTO_MDEBUG - turns on my 'memory leak' detecting stuff
# MFUNC - Make all Malloc/Free/Realloc calls call
#       CRYPTO_malloc/CRYPTO_free/CRYPTO_realloc which can be setup to
#       call application defined callbacks via CRYPTO_set_mem_functions()
# MD5_ASM needs to be defined to use the x86 assembler for MD5
# SHA1_ASM needs to be defined to use the x86 assembler for SHA1
# RMD160_ASM needs to be defined to use the x86 assembler for RIPEMD160
# Do not define B_ENDIAN or L_ENDIAN if 'unsigned long' == 8.  It must
# equal 4.
# PKCS1_CHECK - pkcs1 tests.

#CC= $(MYCC)
CC = $(CROSS_TOOLCHAIN_PATH)/mipsel-linux-gcc
CFLAG += -DOPENSSL_THREADS -D__LINUX__ -O   -mhard-float -fshort-wchar 

ifeq ($(ENABLE_VOOLETV), YES)
CFLAG += -DVOOLETV -D__LINUX__
endif

ifneq ($(TOOLCHAIN_VER), 344)
	CFLAG +=  -fPIC
endif

DEPFLAG= -DOPENSSL_NO_CAMELLIA -DOPENSSL_NO_GMP -DOPENSSL_NO_MDC2 -DOPENSSL_NO_RC5 -DOPENSSL_NO_RFC3779 -DOPENSSL_NO_SEED -DOPENSSL_NO_TLSEXT 
PEX_LIBS= 
EX_LIBS= 
EXE_EXT= 
ARFLAGS= 

AR = $(CROSS_TOOLCHAIN_PATH)/mipsel-linux-ar  $(ARFLAGS) r

#AR=$(MYAR) $(ARFLAGS) r
#RANLIB= $(MYRANLIB)
RANLIB = $(CROSS_TOOLCHAIN_PATH)/mipsel-linux-ranlib


PERL= /usr/bin/perl
TAR= tar
TARFLAGS= --no-recursion
MAKEDEPPROG=makedepend

# We let the C compiler driver to take care of .s files. This is done in
# order to be excused from maintaining a separate set of architecture
# dependent assembler flags. E.g. if you throw -mcpu=ultrasparc at SPARC
# gcc, then the driver will automatically translate it to -xarch=v8plus
# and pass it down to assembler.
AS=$(CC) -c
ASFLAG=$(CFLAG)

# For x86 assembler: Set PROCESSOR to 386 if you want to support
# the 80386.
PROCESSOR= 

# CPUID module collects small commonly used assembler snippets
CPUID_OBJ= 
BN_ASM= bn_asm.o
DES_ENC= des_enc.o fcrypt_b.o
AES_ASM_OBJ= aes_core.o aes_cbc.o
BF_ENC= bf_enc.o
CAST_ENC= c_enc.o
RC4_ENC= rc4_enc.o
RC5_ENC= rc5_enc.o
MD5_ASM_OBJ= 
SHA1_ASM_OBJ= 
RMD160_ASM_OBJ= 

# KRB5 stuff
KRB5_INCLUDES=
LIBKRB5=

# Zlib stuff
ZLIB_INCLUDE=
LIBZLIB=

#DIRS=   crypto ssl engines apps test tools
DIRS=   crypto ssl   
SHLIBDIRS= crypto ssl

# dirs in crypto to build
SDIRS=  \
	objects \
	md2 md4 md5 sha hmac ripemd \
	des aes rc2 rc4 idea bf cast \
	bn ec rsa dsa ecdsa dh ecdh dso engine \
	buffer bio stack lhash rand err \
	evp asn1 pem x509 x509v3 conf txt_db pkcs7 pkcs12 comp ocsp ui krb5 \
	store pqueue
# keep in mind that the above list is adjusted by ./Configure
# according to no-xxx arguments...

# tests to perform.  "alltests" is a special word indicating that all tests
# should be performed.
TESTS = alltests

MAKEFILE= Makefile

MANDIR=$(OPENSSLDIR)/man
MAN1=1
MAN3=3
MANSUFFIX=
SHELL=/bin/sh

TOP=    .
ONEDIRS=out tmp
EDIRS=  times doc bugs util include certs ms shlib mt demos perl sf dep VMS
WDIRS=  windows
LIBS=   libcrypto.a libssl.a
SHARED_CRYPTO=libcrypto$(SHLIB_EXT)
SHARED_SSL=libssl$(SHLIB_EXT)
SHARED_LIBS=
SHARED_LIBS_LINK_EXTS=
SHARED_LDFLAGS=

GENERAL=        Makefile
BASENAME=       openssl
NAME=           $(BASENAME)-$(VERSION)
TARFILE=        $(NAME).tar
WTARFILE=       $(NAME)-win.tar
EXHEADER=       e_os2.h
HEADER=         e_os.h

#PREBUILT_LIB = YES
#ifeq ($(PREBUILT_LIB), YES)
all: Makefile build_all openssl.pc libssl.pc libcrypto.pc
	cp libcrypto.a  $(KWARE_LIBDIR)
	cp libssl.a    $(KWARE_LIBDIR)
#	cp libcrypto.a ../../../lib_release/prebuilt_$(TOOLCHAIN_VER)/
#	cp libssl.a ../../../lib_release/prebuilt_$(TOOLCHAIN_VER)/
#else
#all:
#	echo "skip to build openssl"
#endif


# as we stick to -e, CLEARENV ensures that local variables in lower
# Makefiles remain local and variable. $${VAR+VAR} is tribute to Korn
# shell, which [annoyingly enough] terminates unset with error if VAR
# is not present:-( TOP= && unset TOP is tribute to HP-UX /bin/sh,
# which terminates unset with error if no variable was present:-(
CLEARENV=	TOP= && unset TOP $${LIB+LIB} $${LIBS+LIBS}	\
		$${INCLUDE+INCLUDE} $${INCLUDES+INCLUDES}	\
		$${DIR+DIR} $${DIRS+DIRS} $${SRC+SRC}		\
		$${LIBSRC+LIBSRC} $${LIBOBJ+LIBOBJ} $${ALL+ALL}	\
		$${EXHEADER+EXHEADER} $${HEADER+HEADER}		\
		$${GENERAL+GENERAL} $${CFLAGS+CFLAGS}		\
		$${ASFLAGS+ASFLAGS} $${AFLAGS+AFLAGS}		\
		$${LDCMD+LDCMD} $${LDFLAGS+LDFLAGS}		\
		$${SHAREDCMD+SHAREDCMD} $${SHAREDFLAGS+SHAREDFLAGS}	\
		$${SHARED_LIB+SHARED_LIB} $${LIBEXTRAS+LIBEXTRAS}

BUILDENV=	PLATFORM='${PLATFORM}' PROCESSOR='${PROCESSOR}' \
		CC='${CC}' CFLAG='${CFLAG}' 			\
		AS='${CC}' ASFLAG='${CFLAG} -c'			\
		AR='${AR}' PERL='${PERL}' RANLIB='${RANLIB}'	\
		SDIRS='${SDIRS}' LIBRPATH='${INSTALLTOP}/lib'	\
		INSTALL_PREFIX='${INSTALL_PREFIX}'		\
		INSTALLTOP='${INSTALLTOP}' OPENSSLDIR='${OPENSSLDIR}'	\
		MAKEDEPEND='$$$${TOP}/util/domd $$$${TOP} -MD ${MAKEDEPPROG}' \
		DEPFLAG='-DOPENSSL_NO_DEPRECATED ${DEPFLAG}'	\
		MAKEDEPPROG='${MAKEDEPPROG}'			\
		SHARED_LDFLAGS='${SHARED_LDFLAGS}'		\
		KRB5_INCLUDES='${KRB5_INCLUDES}' LIBKRB5='${LIBKRB5}'	\
		EXE_EXT='${EXE_EXT}' SHARED_LIBS='${SHARED_LIBS}'	\
		SHLIB_EXT='${SHLIB_EXT}' SHLIB_TARGET='${SHLIB_TARGET}'	\
		PEX_LIBS='${PEX_LIBS}' EX_LIBS='${EX_LIBS}'	\
		CPUID_OBJ='${CPUID_OBJ}'			\
		BN_ASM='${BN_ASM}' DES_ENC='${DES_ENC}' 	\
		AES_ASM_OBJ='${AES_ASM_OBJ}'			\
		BF_ENC='${BF_ENC}' CAST_ENC='${CAST_ENC}'	\
		RC4_ENC='${RC4_ENC}' RC5_ENC='${RC5_ENC}'	\
		SHA1_ASM_OBJ='${SHA1_ASM_OBJ}'			\
		MD5_ASM_OBJ='${MD5_ASM_OBJ}'			\
		RMD160_ASM_OBJ='${RMD160_ASM_OBJ}'		\
		THIS=$${THIS:-$@} MAKEFILE=Makefile MAKEOVERRIDES=
# MAKEOVERRIDES= effectively "equalizes" GNU-ish and SysV-ish make flavors,
# which in turn eliminates ambiguities in variable treatment with -e.

# BUILD_CMD is a generic macro to build a given target in a given
# subdirectory.  The target must be given through the shell variable
# `target' and the subdirectory to build in must be given through `dir'.
# This macro shouldn't be used directly, use RECURSIVE_BUILD_CMD or
# BUILD_ONE_CMD instead.
#
# BUILD_ONE_CMD is a macro to build a given target in a given
# subdirectory if that subdirectory is part of $(DIRS).  It requires
# exactly the same shell variables as BUILD_CMD.
#
# RECURSIVE_BUILD_CMD is a macro to build a given target in all
# subdirectories defined in $(DIRS).  It requires that the target
# is given through the shell variable `target'.
BUILD_CMD=  if [ -d "$$dir" ]; then \
	    (	cd $$dir && echo "making $$target in $$dir..." && \
		$(CLEARENV) && $(MAKE) -e $(BUILDENV) TOP=.. DIR=$$dir $$target \
	    ) || exit 1; \
	    fi
RECURSIVE_BUILD_CMD=for dir in $(DIRS); do $(BUILD_CMD); done
BUILD_ONE_CMD=\
	if echo " $(DIRS) " | grep " $$dir " >/dev/null 2>/dev/null; then \
		$(BUILD_CMD); \
	fi

reflect:
	@[ -n "$(THIS)" ] && $(CLEARENV) && $(MAKE) $(THIS) -e $(BUILDENV)

sub_all: build_all
build_all: build_libs build_apps build_tests build_tools

build_libs: build_crypto build_ssl build_engines

build_crypto:
	@dir=crypto; target=all; $(BUILD_ONE_CMD)
build_ssl:
	@dir=ssl; target=all; $(BUILD_ONE_CMD)
build_engines:
	@dir=engines; target=all; $(BUILD_ONE_CMD)
build_apps:
	@dir=apps; target=all; $(BUILD_ONE_CMD)
build_tests:
	@dir=test; target=all; $(BUILD_ONE_CMD)
build_tools:
	@dir=tools; target=all; $(BUILD_ONE_CMD)

all_testapps: build_libs build_testapps
build_testapps:
	@dir=crypto; target=testapps; $(BUILD_ONE_CMD)

libcrypto$(SHLIB_EXT): libcrypto.a
	@if [ "$(SHLIB_TARGET)" != "" ]; then \
		$(MAKE) SHLIBDIRS=crypto build-shared; \
	else \
		echo "There's no support for shared libraries on this platform" >&2; \
		exit 1; \
	fi

libssl$(SHLIB_EXT): libcrypto$(SHLIB_EXT) libssl.a
	@if [ "$(SHLIB_TARGET)" != "" ]; then \
		$(MAKE) SHLIBDIRS=ssl SHLIBDEPS='-lcrypto' build-shared; \
	else \
		echo "There's no support for shared libraries on this platform" >&2; \
		exit 1; \
	fi

clean-shared:
	@set -e; for i in $(SHLIBDIRS); do \
		if [ -n "$(SHARED_LIBS_LINK_EXTS)" ]; then \
			tmp="$(SHARED_LIBS_LINK_EXTS)"; \
			for j in $${tmp:-x}; do \
				( set -x; rm -f lib$$i$$j ); \
			done; \
		fi; \
		( set -x; rm -f lib$$i$(SHLIB_EXT) ); \
		if [ "$(PLATFORM)" = "Cygwin" ]; then \
			( set -x; rm -f cyg$$i$(SHLIB_EXT) lib$$i$(SHLIB_EXT).a ); \
		fi; \
	done

link-shared:
	@ set -e; for i in ${SHLIBDIRS}; do \
		$(MAKE) -f $(HERE)/Makefile.shared -e $(BUILDENV) \
			LIBNAME=$$i LIBVERSION=${SHLIB_MAJOR}.${SHLIB_MINOR} \
			LIBCOMPATVERSIONS=";${SHLIB_VERSION_HISTORY}" \
			symlink.$(SHLIB_TARGET); \
		libs="$$libs -l$$i"; \
	done

build-shared: do_$(SHLIB_TARGET) link-shared

do_$(SHLIB_TARGET):
	@ set -e; libs='-L. ${SHLIBDEPS}'; for i in ${SHLIBDIRS}; do \
		if [ "${SHLIBDIRS}" = "ssl" -a -n "$(LIBKRB5)" ]; then \
			libs="$(LIBKRB5) $$libs"; \
		fi; \
		$(CLEARENV) && $(MAKE) -f Makefile.shared -e $(BUILDENV) \
			LIBNAME=$$i LIBVERSION=${SHLIB_MAJOR}.${SHLIB_MINOR} \
			LIBCOMPATVERSIONS=";${SHLIB_VERSION_HISTORY}" \
			LIBDEPS="$$libs $(EX_LIBS)" \
			link_a.$(SHLIB_TARGET); \
		libs="-l$$i $$libs"; \
	done

libcrypto.pc: Makefile
	@ ( echo 'prefix=$(INSTALLTOP)'; \
	    echo 'exec_prefix=$${prefix}'; \
	    echo 'libdir=$${exec_prefix}/lib'; \
	    echo 'includedir=$${prefix}/include'; \
	    echo ''; \
	    echo 'Name: OpenSSL-libcrypto'; \
	    echo 'Description: OpenSSL cryptography library'; \
	    echo 'Version: '$(VERSION); \
	    echo 'Requires: '; \
	    echo 'Libs: -L$${libdir} -lcrypto $(EX_LIBS)'; \
	    echo 'Cflags: -I$${includedir} $(KRB5_INCLUDES)' ) > libcrypto.pc

libssl.pc: Makefile
	@ ( echo 'prefix=$(INSTALLTOP)'; \
	    echo 'exec_prefix=$${prefix}'; \
	    echo 'libdir=$${exec_prefix}/lib'; \
	    echo 'includedir=$${prefix}/include'; \
	    echo ''; \
	    echo 'Name: OpenSSL'; \
	    echo 'Description: Secure Sockets Layer and cryptography libraries'; \
	    echo 'Version: '$(VERSION); \
	    echo 'Requires: '; \
	    echo 'Libs: -L$${libdir} -lssl -lcrypto $(EX_LIBS)'; \
	    echo 'Cflags: -I$${includedir} $(KRB5_INCLUDES)' ) > libssl.pc

openssl.pc: Makefile
	@ ( echo 'prefix=$(INSTALLTOP)'; \
	    echo 'exec_prefix=$${prefix}'; \
	    echo 'libdir=$${exec_prefix}/lib'; \
	    echo 'includedir=$${prefix}/include'; \
	    echo ''; \
	    echo 'Name: OpenSSL'; \
	    echo 'Description: Secure Sockets Layer and cryptography libraries and tools'; \
	    echo 'Version: '$(VERSION); \
	    echo 'Requires: '; \
	    echo 'Libs: -L$${libdir} -lssl -lcrypto $(EX_LIBS)'; \
	    echo 'Cflags: -I$${includedir} $(KRB5_INCLUDES)' ) > openssl.pc

Makefile: 
	@echo "Makefile is older than Makefile.org, Configure or config."
	@echo "Reconfigure the source tree (via './config' or 'perl Configure'), please."
	@false

libclean:
	rm -f *.map *.so *.so.* *.dll engines/*.so engines/*.dll *.a engines/*.a */lib */*/lib

clean:	libclean
	rm -f shlib/*.o *.o core a.out fluff rehash.time testlog make.log cctest cctest.c
	@set -e; target=clean; $(RECURSIVE_BUILD_CMD)
	rm -f $(LIBS)
	rm -f openssl.pc libssl.pc libcrypto.pc
	rm -f speed.* .pure
	rm -f $(TARFILE)
	@set -e; for i in $(ONEDIRS) ;\
	do \
	rm -fr $$i/*; \
	done

makefile.one: files
	$(PERL) util/mk1mf.pl >makefile.one; \
	sh util/do_ms.sh

files:
	$(PERL) $(TOP)/util/files.pl Makefile > $(TOP)/MINFO
	@set -e; target=files; $(RECURSIVE_BUILD_CMD)

links:
	@$(PERL) $(TOP)/util/mkdir-p.pl include/openssl
	@$(PERL) $(TOP)/util/mklink.pl include/openssl $(EXHEADER)
	@set -e; target=links; $(RECURSIVE_BUILD_CMD)

gentests:
	@(cd test && echo "generating dummy tests (if needed)..." && \
	$(CLEARENV) && $(MAKE) -e $(BUILDENV) TESTS='${TESTS}' OPENSSL_DEBUG_MEMORY=on generate );

dclean:
	rm -f *.bak
	@set -e; target=dclean; $(RECURSIVE_BUILD_CMD)

rehash: rehash.time
rehash.time: certs
	@(OPENSSL="`pwd`/util/opensslwrap.sh"; \
	  OPENSSL_DEBUG_MEMORY=on; \
	  export OPENSSL OPENSSL_DEBUG_MEMORY; \
	  $(PERL) tools/c_rehash certs)
	touch rehash.time

test:   tests

tests: rehash
	@(cd test && echo "testing..." && \
	$(CLEARENV) && $(MAKE) -e $(BUILDENV) TOP=.. TESTS='${TESTS}' OPENSSL_DEBUG_MEMORY=on tests );
	util/opensslwrap.sh version -a

report:
	@$(PERL) util/selftest.pl

depend:
	@set -e; target=depend; $(RECURSIVE_BUILD_CMD)

lint:
	@set -e; target=lint; $(RECURSIVE_BUILD_CMD)

tags:
	rm -f TAGS
	find . -name '[^.]*.[ch]' | xargs etags -a

errors:
	$(PERL) util/mkerr.pl -recurse -write
	(cd engines; $(MAKE) PERL=$(PERL) errors)
	$(PERL) util/ck_errf.pl */*.c */*/*.c

stacks:
	$(PERL) util/mkstack.pl -write

util/libeay.num::
	$(PERL) util/mkdef.pl crypto update

util/ssleay.num::
	$(PERL) util/mkdef.pl ssl update

crypto/objects/obj_dat.h: crypto/objects/obj_dat.pl crypto/objects/obj_mac.h
	$(PERL) crypto/objects/obj_dat.pl crypto/objects/obj_mac.h crypto/objects/obj_dat.h
crypto/objects/obj_mac.h: crypto/objects/objects.pl crypto/objects/objects.txt crypto/objects/obj_mac.num
	$(PERL) crypto/objects/objects.pl crypto/objects/objects.txt crypto/objects/obj_mac.num crypto/objects/obj_mac.h

apps/openssl-vms.cnf: apps/openssl.cnf
	$(PERL) VMS/VMSify-conf.pl < apps/openssl.cnf > apps/openssl-vms.cnf

crypto/bn/bn_prime.h: crypto/bn/bn_prime.pl
	$(PERL) crypto/bn/bn_prime.pl >crypto/bn/bn_prime.h


TABLE: Configure
	(echo 'Output of `Configure TABLE'"':"; \
	$(PERL) Configure TABLE) > TABLE

update: errors stacks util/libeay.num util/ssleay.num crypto/objects/obj_dat.h apps/openssl-vms.cnf crypto/bn/bn_prime.h TABLE depend

# Build distribution tar-file. As the list of files returned by "find" is
# pretty long, on several platforms a "too many arguments" error or similar
# would occur. Therefore the list of files is temporarily stored into a file
# and read directly, requiring GNU-Tar. Call "make TAR=gtar dist" if the normal
# tar does not support the --files-from option.
tar:
	find . -type d -print | xargs chmod 755
	find . -type f -print | xargs chmod a+r
	find . -type f -perm -0100 -print | xargs chmod a+x
	find * \! -path CVS/\* \! -path \*/CVS/\* \! -name CVS \! -name .cvsignore \! -name STATUS \! -name TABLE | sort > ../$(TARFILE).list; \
	$(TAR) $(TARFLAGS) --files-from ../$(TARFILE).list -cvf - | \
	tardy --user_number=0  --user_name=openssl \
	      --group_number=0 --group_name=openssl \
	      --prefix=openssl-$(VERSION) - |\
	gzip --best >../$(TARFILE).gz; \
	rm -f ../$(TARFILE).list; \
	ls -l ../$(TARFILE).gz

tar-snap:
	@$(TAR) $(TARFLAGS) -cvf - \
		`find * \! -path CVS/\* \! -path \*/CVS/\* \! -name CVS \! -name .cvsignore \! -name STATUS \! -name TABLE \! -name '*.o' \! -name '*.a' \! -name '*.so' \! -name '*.so.*'  \! -name 'openssl' \! -name '*test' \! -name '.#*' \! -name '*~' | sort` |\
	tardy --user_number=0  --user_name=openssl \
	      --group_number=0 --group_name=openssl \
	      --prefix=openssl-$(VERSION) - > ../$(TARFILE);\
	ls -l ../$(TARFILE)

dist:   
	$(PERL) Configure dist
	@$(MAKE) dist_pem_h
	@$(MAKE) SDIRS='${SDIRS}' clean
	@$(MAKE) TAR='${TAR}' TARFLAGS='${TARFLAGS}' tar

dist_pem_h:
	(cd crypto/pem; $(MAKE) -e $(BUILDENV) pem.h; $(MAKE) clean)

install: all install_docs install_sw

install_sw:
	@$(PERL) $(TOP)/util/mkdir-p.pl $(INSTALL_PREFIX)$(INSTALLTOP)/bin \
		$(INSTALL_PREFIX)$(INSTALLTOP)/lib \
		$(INSTALL_PREFIX)$(INSTALLTOP)/lib/engines \
		$(INSTALL_PREFIX)$(INSTALLTOP)/lib/pkgconfig \
		$(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl \
		$(INSTALL_PREFIX)$(OPENSSLDIR)/misc \
		$(INSTALL_PREFIX)$(OPENSSLDIR)/certs \
		$(INSTALL_PREFIX)$(OPENSSLDIR)/private
	@set -e; headerlist="$(EXHEADER)"; for i in $$headerlist;\
	do \
	(cp $$i $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i; \
	chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i ); \
	done;
	@set -e; target=install; $(RECURSIVE_BUILD_CMD)
	@set -e; for i in $(LIBS) ;\
	do \
		if [ -f "$$i" ]; then \
		(       echo installing $$i; \
			cp $$i $(INSTALL_PREFIX)$(INSTALLTOP)/lib/$$i.new; \
			$(RANLIB) $(INSTALL_PREFIX)$(INSTALLTOP)/lib/$$i.new; \
			chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/lib/$$i.new; \
			mv -f $(INSTALL_PREFIX)$(INSTALLTOP)/lib/$$i.new $(INSTALL_PREFIX)$(INSTALLTOP)/lib/$$i ); \
		fi; \
	done;
	@set -e; if [ -n "$(SHARED_LIBS)" ]; then \
		tmp="$(SHARED_LIBS)"; \
		for i in $${tmp:-x}; \
		do \
			if [ -f "$$i" -o -f "$$i.a" ]; then \
			(       echo installing $$i; \
				if [ "$(PLATFORM)" != "Cygwin" ]; then \
					cp $$i $(INSTALL_PREFIX)$(INSTALLTOP)/lib/$$i.new; \
					chmod 555 $(INSTALL_PREFIX)$(INSTALLTOP)/lib/$$i.new; \
					mv -f $(INSTALL_PREFIX)$(INSTALLTOP)/lib/$$i.new $(INSTALL_PREFIX)$(INSTALLTOP)/lib/$$i; \
				else \
					c=`echo $$i | sed 's/^lib\(.*\)\.dll\.a/cyg\1-$(SHLIB_VERSION_NUMBER).dll/'`; \
					cp $$c $(INSTALL_PREFIX)$(INSTALLTOP)/bin/$$c.new; \
					chmod 755 $(INSTALL_PREFIX)$(INSTALLTOP)/bin/$$c.new; \
					mv -f $(INSTALL_PREFIX)$(INSTALLTOP)/bin/$$c.new $(INSTALL_PREFIX)$(INSTALLTOP)/bin/$$c; \
					cp $$i $(INSTALL_PREFIX)$(INSTALLTOP)/lib/$$i.new; \
					chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/lib/$$i.new; \
					mv -f $(INSTALL_PREFIX)$(INSTALLTOP)/lib/$$i.new $(INSTALL_PREFIX)$(INSTALLTOP)/lib/$$i; \
				fi ); \
			fi; \
		done; \
		(	here="`pwd`"; \
			cd $(INSTALL_PREFIX)$(INSTALLTOP)/lib; \
			$(MAKE) -f $$here/Makefile HERE="$$here" link-shared ); \
		if [ "$(INSTALLTOP)" != "/usr" ]; then \
			echo 'OpenSSL shared libraries have been installed in:'; \
			echo '  $(INSTALLTOP)'; \
			echo ''; \
			sed -e '1,/^$$/d' doc/openssl-shared.txt; \
		fi; \
	fi
	cp libcrypto.pc $(INSTALL_PREFIX)$(INSTALLTOP)/lib/pkgconfig
	chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/lib/pkgconfig/libcrypto.pc
	cp libssl.pc $(INSTALL_PREFIX)$(INSTALLTOP)/lib/pkgconfig
	chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/lib/pkgconfig/libssl.pc
	cp openssl.pc $(INSTALL_PREFIX)$(INSTALLTOP)/lib/pkgconfig
	chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/lib/pkgconfig/openssl.pc

install_docs:
	@$(PERL) $(TOP)/util/mkdir-p.pl \
		$(INSTALL_PREFIX)$(MANDIR)/man1 \
		$(INSTALL_PREFIX)$(MANDIR)/man3 \
		$(INSTALL_PREFIX)$(MANDIR)/man5 \
		$(INSTALL_PREFIX)$(MANDIR)/man7
	@pod2man="`cd util; ./pod2mantest $(PERL)`"; \
	here="`pwd`"; \
	filecase=; \
	if [ "$(PLATFORM)" = "DJGPP" -o "$(PLATFORM)" = "Cygwin" -o "$(PLATFORM)" = "mingw" ]; then \
		filecase=-i; \
	fi; \
	set -e; for i in doc/apps/*.pod; do \
		fn=`basename $$i .pod`; \
		sec=`$(PERL) util/extract-section.pl 1 < $$i`; \
		echo "installing man$$sec/$$fn.$${sec}$(MANSUFFIX)"; \
		(cd `$(PERL) util/dirname.pl $$i`; \
		sh -c "$$pod2man \
			--section=$$sec --center=OpenSSL \
			--release=$(VERSION) `basename $$i`") \
			>  $(INSTALL_PREFIX)$(MANDIR)/man$$sec/$$fn.$${sec}$(MANSUFFIX); \
		$(PERL) util/extract-names.pl < $$i | \
			(grep -v $$filecase "^$$fn\$$"; true) | \
			(grep -v "[	]"; true) | \
			(cd $(INSTALL_PREFIX)$(MANDIR)/man$$sec/; \
			 while read n; do \
				$$here/util/point.sh $$fn.$${sec}$(MANSUFFIX) "$$n".$${sec}$(MANSUFFIX); \
			 done); \
	done; \
	set -e; for i in doc/crypto/*.pod doc/ssl/*.pod; do \
		fn=`basename $$i .pod`; \
		sec=`$(PERL) util/extract-section.pl 3 < $$i`; \
		echo "installing man$$sec/$$fn.$${sec}$(MANSUFFIX)"; \
		(cd `$(PERL) util/dirname.pl $$i`; \
		sh -c "$$pod2man \
			--section=$$sec --center=OpenSSL \
			--release=$(VERSION) `basename $$i`") \
			>  $(INSTALL_PREFIX)$(MANDIR)/man$$sec/$$fn.$${sec}$(MANSUFFIX); \
		$(PERL) util/extract-names.pl < $$i | \
			(grep -v $$filecase "^$$fn\$$"; true) | \
			(grep -v "[	]"; true) | \
			(cd $(INSTALL_PREFIX)$(MANDIR)/man$$sec/; \
			 while read n; do \
				$$here/util/point.sh $$fn.$${sec}$(MANSUFFIX) "$$n".$${sec}$(MANSUFFIX); \
			 done); \
	done

# DO NOT DELETE THIS LINE -- make depend depends on it.
endif