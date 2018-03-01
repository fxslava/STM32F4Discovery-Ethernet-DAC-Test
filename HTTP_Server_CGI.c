/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server_CGI.c
 * Purpose: HTTP Server CGI Module
 * Rev.:    V6.00
 *----------------------------------------------------------------------------*/
//! [code_HTTP_Server_CGI]
#include <stdio.h>
#include <string.h>
#include "rl_net.h"
 
// Process query string received by GET request.
void cgi_process_query (const char *qstr) {
  char var[40];

  do {
    // Loop through all the parameters
    qstr = http_get_env_var (qstr, var, sizeof (var));
    // Check return string, 'qstr' now points to the next parameter
    if (var[0] != 0) {
      // First character is non-null, string exists

      /* Example of a parameter "ip=192.168.0.100"
      if (strncmp (var, "ip=", 3) == 0) {
        uint8_t ip_addr[IP4_ADDR_LEN];
        // Read parameter IP address submitted by the browser
        ip4_aton (&var[3], ip_addr);
         ...
      }
      */
    }
  } while (qstr);
}
 
// Process data received by POST request.
// Type code: - 0 = www-url-encoded form data.
//            - 1 = filename for file upload (null-terminated string).
//            - 2 = file upload raw data.
//            - 3 = end of file upload (file close requested).
//            - 4 = any XML encoded POST data (single or last stream).
//            - 5 = the same as 4, but with more XML data to follow.
void cgi_process_data (uint8_t code, const char *data, uint32_t len) {
  char var[40];
 
  switch (code) {
    case 0:
      // Url encoded form data received
      do {
        // Parse all parameters
        data = http_get_env_var (data, var, sizeof (var));
        if (var[0] != 0) {
          // First character is non-null, string exists
          /* Example
          if (strcmp (var, "led0=on") == 0) {
            // ... Switch LED 0 on
          }
          */
        }
      } while (data);
      break;

    case 1:
      // Filename for file upload received
      /* Example
      if (data[0] != 0) {
        // Open a file for writing
        file = fopen (var, "w");
      }
      */
      break;

    case 2:
      // File content data received
      /* Example
      if (file != NULL) {
        // Write data to a file
        fwrite (data, 1, len, file);
      }
      */
      break;

    case 3:
      // File upload finished
      /* Example
      if (file != NULL) {
        // Close a file
        fclose (file);
      }
      */
      break;

    case 4:
      // XML encoded content type, last packet
      /* Example
      if (strcmp (http_get_content_type(), "text/xml; charset=utf-8" == 0) {
        // Content type xml, utf-8 encoding
         ...
      }
      */
      break;

    case 5:
      // XML encoded content type, more to follow
      // ... Process xml data
      break;

    default:
      // Ignore all other codes
      break;
  }
}
 
// Generate dynamic web data from a script line.
uint32_t cgi_script (const char *env, char *buf, uint32_t buflen, uint32_t *pcgi) {
  uint32_t len = 0;
 
  // Analyze a 'c' script line starting position 2
  /* Example script
  // c a i <td><input type=text name=ip value="%s" size=18 maxlength=18></td>

  switch (env[0]) {
    case 'a' :
      switch (env[2]) {
        case 'i':
          // Write the local IP address
          len = sprintf (buf, &env[4], ip4_ntoa (IpAddr));
          break;
          ...
      }
      break;
  }
  */
  return (len);
}
 
// Override default Content-Type for CGX script files.
const char *cgx_content_type (void) {
  /* Example
  return ("text/xml; charset=utf-8");
  */
  return (NULL);
}
 
// Override default character encoding in html documents.
const char *http_encoding (void) {
  /* Example
  return ("utf-8");
  */
  return (NULL);
}
//! [code_HTTP_Server_CGI]
