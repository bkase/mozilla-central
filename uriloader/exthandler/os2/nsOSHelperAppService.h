/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is the Mozilla browser.
 * 
 * The Initial Developer of the Original Code is Netscape
 * Communications, Inc.  Portions created by Netscape are
 * Copyright (C) 1999, Mozilla.  All Rights Reserved.
 * 
 * Contributor(s):
 *   Scott MacGregor <mscott@netscape.com>
 */

#ifndef nsOSHelperAppService_h__
#define nsOSHelperAppService_h__

// The OS helper app service is a subclass of nsExternalHelperAppService and is implemented on each
// platform. It contains platform specific code for finding helper applications for a given mime type
// in addition to launching those applications.

#include "nsExternalHelperAppService.h"
#include "nsCExternalHandlerService.h"
#include "nsCOMPtr.h"

class nsOSHelperAppService : public nsExternalHelperAppService
{
public:
  nsOSHelperAppService();
  virtual ~nsOSHelperAppService();

  // override nsIExternalHelperAppService methods....
  NS_IMETHOD CanHandleContent(const char *aMimeContentType, nsIURI * aURI, PRBool *_retval);
  NS_IMETHOD DoContent(const char *aMimeContentType, nsIURI *aURI, nsISupports *aWindowContext, PRBool *aAbortProcess, nsIStreamListener **_retval);
  NS_IMETHOD LaunchAppWithTempFile(nsIFile * aTempFile, nsISupports * aAppCookie);

  // override nsIExternalProtocolService methods
  NS_IMETHOD ExternalProtocolHandlerExists(const char * aProtocolScheme, PRBool * aHandlerExists);
  NS_IMETHOD LoadUrl(nsIURI * aURL);

  // GetFileTokenForPath must be implemented by each platform. 
  // platformAppPath --> a platform specific path to an application that we got out of the 
  //                     rdf data source. This can be a mac file spec, a unix path or a windows path depending on the platform
  // aFile --> an nsIFile representation of that platform application path.
  virtual nsresult GetFileTokenForPath(const PRUnichar * platformAppPath, nsIFile ** aFile);
  
  // CreateStreamListenerWithApp --> must be implemented by each platform.
  // aApplicationToUse --> the application the user wishes to launch with the incoming data
  // aFileExtensionForData --> the extension we are going to use for the temp file in the external app handler
  // aStreamListener --> the stream listener (really a external app handler) we're going to use for retrieving the data
  virtual nsresult CreateStreamListenerWithApp(nsIFile * aApplicationToUse, const char * aFileExtensionForData, nsIStreamListener ** aStreamListener);


protected:

};

#endif // nsOSHelperAppService_h__
