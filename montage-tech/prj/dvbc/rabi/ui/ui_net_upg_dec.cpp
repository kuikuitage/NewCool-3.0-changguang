/****************************************************************************

 ****************************************************************************/

#include "sys_types.h"

#include "tinystr.h"
#include "tinyxml.h"

#include "ui_net_upg_dec.h"

BOOL ui_net_upg_info_parse(u8 *p_data, u32 customer_id, u32 serial_num, u32 software_version, net_upg_info_t *p_out)
{
  TiXmlDocument * p_xml = NULL;
  TiXmlElement* element = NULL;
  const char * p_tmp  = NULL;
  TiXmlElement *p_item = NULL;
  TiXmlElement *p_sub = NULL;
  u8 i = 0;

  p_xml = new TiXmlDocument();
  p_xml->Parse((char *)p_data, 0, TIXML_ENCODING_UTF8);

  TiXmlHandle hDoc((TiXmlDocument *)p_xml);
  element = hDoc.FirstChild("entry").FirstChild("customer").Element();

  if(element != NULL)
  {
    p_tmp = element->Attribute("attribute");
    memcpy(p_out->customer_name, p_tmp, sizeof(p_out->customer_name)-1);

    p_item = (TiXmlElement *)element->FirstChild("item");
    while(p_item != NULL)
    {
      p_sub = (TiXmlElement *)p_item->FirstChild("customer_id");
      if(p_sub)
      {
        p_tmp = p_sub->GetText();
        p_out->item[i].customer_id = atoi(p_tmp);
      }
      else
        p_out->item[i].customer_id = 0;

      if(p_out->item[i].customer_id!=customer_id)
      {
        p_item = p_item->NextSiblingElement("item");
        continue;
      }

      p_sub = (TiXmlElement *)p_item->FirstChild("serial_start");
      if(p_sub)
      {
        p_tmp = p_sub->GetText();
        p_out->item[i].serial_start = atoi(p_tmp);
      }
      else
        p_out->item[i].serial_start = 0;	  	

      p_sub = (TiXmlElement *)p_item->FirstChild("serial_end");
      if(p_sub)
      {
        p_tmp = p_sub->GetText();
        p_out->item[i].serial_end = atoi(p_tmp);
      }
      else
        p_out->item[i].serial_end = 0;	  	

      if(serial_num > 0 && serial_num >= p_out->item[i].serial_start
           &&serial_num <= p_out->item[i].serial_end)
        ;
      else if(serial_num == 0)
        ;
      else
      {
        p_item = p_item->NextSiblingElement("item");
        continue;
      }

      p_sub = (TiXmlElement *)p_item->FirstChild("hw");
      if(p_sub)
      {
        p_tmp = p_sub->GetText();
        p_out->item[i].hw = atoi(p_tmp);
      }

      p_sub = (TiXmlElement *)p_item->FirstChild("sw");
      if(p_sub)
      {
        p_tmp = p_sub->GetText();
        p_out->item[i].sw = atoi(p_tmp);
      }

      if(p_out->item[i].sw <= software_version)
      {
        p_item = p_item->NextSiblingElement("item");
        continue;
      }

      p_sub = (TiXmlElement *)p_item->FirstChild("url");
      if(p_sub)
      {
        p_tmp = p_sub->GetText();
        memcpy(p_out->item[i].url, p_tmp, strlen(p_tmp));
      }

      p_sub = (TiXmlElement *)p_item->FirstChild("size");
      if(p_sub)
      {
        p_tmp = p_sub->GetText();
        p_out->item[i].size = atoi(p_tmp);
      }

      p_sub = (TiXmlElement *)p_item->FirstChild("name");
      if(p_sub)
      {
        p_tmp = p_sub->GetText();
        memcpy(p_out->item[i].name, p_tmp, strlen(p_tmp));
      }

      i ++;
      p_item = p_item->NextSiblingElement("item");
    }
    p_out->item_cnt = i;
  }
  
  delete p_xml;
  
  return TRUE;
}

