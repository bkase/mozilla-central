/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Mozilla Communicator client code.
 *
 * The Initial Developer of the Original Code is Netscape Communications
 * Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation. All
 * Rights Reserved.
 *
 * Contributor(s): 
 */

#include "nsIComponentManager.h"
#include "nsIServiceManager.h"
#include "rdf.h"
#include "nsIRDFDataSource.h"
#include "nsXPIDLString.h"
#include "nsIRDFContainerUtils.h"
#include "nsIRDFService.h"
#include "nsRDFCID.h"
#include "nsUConvDll.h"
#include "nsCharsetMenu.h"
#include "nsICharsetConverterManager.h"
#include "nsIStringBundle.h"
#include "nsILocaleFactory.h"
#include "nsLocaleCID.h"

static NS_DEFINE_IID(kIRDFServiceIID, NS_IRDFSERVICE_IID);
static NS_DEFINE_CID(kRDFServiceCID, NS_RDFSERVICE_CID);
static NS_DEFINE_CID(kRDFInMemoryDataSourceCID, NS_RDFINMEMORYDATASOURCE_CID);
static NS_DEFINE_IID(kIRDFDataSourceIID, NS_IRDFDATASOURCE_IID);
static NS_DEFINE_CID(kIRDFContainerUtilsIID, NS_IRDFCONTAINERUTILS_IID);
static NS_DEFINE_CID(kRDFContainerUtilsCID, NS_RDFCONTAINERUTILS_CID);
static NS_DEFINE_CID(kRDFContainerCID, NS_RDFCONTAINER_CID);
static NS_DEFINE_CID(kCharsetConverterManagerCID, NS_ICHARSETCONVERTERMANAGER_CID);
static NS_DEFINE_IID(kICharsetConverterManagerIID, NS_ICHARSETCONVERTERMANAGER_IID);
static NS_DEFINE_CID(kStringBundleServiceCID, NS_STRINGBUNDLESERVICE_CID);
static NS_DEFINE_CID(kLocaleFactoryCID, NS_LOCALEFACTORY_CID);

static const char * kURINC_BSCharsetMenuRoot = "NC:BSCharsetMenuRoot";
static const char * kURINC_BDCharsetMenuRoot = "NC:BDCharsetMenuRoot";
static const char * kURINC_BMCharsetMenuRoot = "NC:BMCharsetMenuRoot";
DEFINE_RDF_VOCAB(NC_NAMESPACE_URI, NC, Name);

//----------------------------------------------------------------------------
// Class nsCharsetMenu [declaration]

/**
 * The Charset Converter menu.
 *
 * @created         23/Nov/1999
 * @author  Catalin Rotaru [CATA]
 */
class nsCharsetMenu : public nsIRDFDataSource
{
  NS_DECL_ISUPPORTS

private:
  static nsIRDFResource * kNC_BSCharsetMenuRoot;
  static nsIRDFResource * kNC_BDCharsetMenuRoot;
  static nsIRDFResource * kNC_BMCharsetMenuRoot;
  static nsIRDFResource * kNC_Name;

  static nsIRDFDataSource * mInner;

  nsresult Init();
  nsresult Done();
  nsresult CreateBrowserMenu();
  nsresult CreateBrowserMoreMenu(nsIRDFService * aRDFServ, 
      nsIStringBundle * aTitles, nsIStringBundle * aData);
  nsresult CreateBrowserStaticMenu(nsIRDFService * aRDFServ, 
      nsIStringBundle * aTitles, nsIStringBundle * aData);
  nsresult FillRDFContainer(nsIRDFService * aRDFServ, nsIRDFResource * aResource, 
      nsIStringBundle * aTitles, nsString ** aList, PRInt32 aCount);
  nsresult RemoveFlaggedCharsets(nsString ** aList, PRInt32 aCount, 
      nsIStringBundle * aData, char * aProp, PRBool value);

  nsresult NewRDFContainer(nsIRDFDataSource * aDataSource, 
      nsIRDFResource * aResource, nsIRDFContainer ** aResult);

  nsresult GetAppLocale(nsILocale ** aLocale);

public:
  nsCharsetMenu();
  virtual ~nsCharsetMenu();

  //--------------------------------------------------------------------------
  // Interface nsIRDFDataSource [declaration]

  NS_IMETHOD GetURI(char ** uri);

  NS_IMETHOD GetSource(nsIRDFResource* property, nsIRDFNode* target, 
      PRBool tv, nsIRDFResource** source);

  NS_IMETHOD GetSources(nsIRDFResource* property, nsIRDFNode* target, 
      PRBool tv, nsISimpleEnumerator** sources);

  NS_IMETHOD GetTarget(nsIRDFResource* source, nsIRDFResource* property, 
      PRBool tv, nsIRDFNode** target);

  NS_IMETHOD GetTargets(nsIRDFResource* source, nsIRDFResource* property, 
      PRBool tv, nsISimpleEnumerator** targets);

  NS_IMETHOD Assert(nsIRDFResource* aSource, nsIRDFResource* aProperty, 
      nsIRDFNode* aTarget, PRBool aTruthValue);

  NS_IMETHOD Unassert(nsIRDFResource* aSource, nsIRDFResource* aProperty, 
      nsIRDFNode* aTarget);

  NS_IMETHOD Change(nsIRDFResource* aSource, nsIRDFResource* aProperty, 
      nsIRDFNode* aOldTarget, nsIRDFNode* aNewTarget);

  NS_IMETHOD Move(nsIRDFResource* aOldSource, nsIRDFResource* aNewSource, 
      nsIRDFResource* aProperty, nsIRDFNode* aTarget);

  NS_IMETHOD HasAssertion(nsIRDFResource* source, nsIRDFResource* property, 
      nsIRDFNode* target, PRBool tv, PRBool* hasAssertion);

  NS_IMETHOD AddObserver(nsIRDFObserver* n);

  NS_IMETHOD RemoveObserver(nsIRDFObserver* n);

  NS_IMETHOD ArcLabelsIn( nsIRDFNode* node, nsISimpleEnumerator** labels);

  NS_IMETHOD ArcLabelsOut(nsIRDFResource* source, 
      nsISimpleEnumerator** labels);

  NS_IMETHOD GetAllResources(nsISimpleEnumerator** aCursor);

  NS_IMETHOD GetAllCommands(nsIRDFResource* source, nsIEnumerator** commands);

  NS_IMETHOD GetAllCmds(nsIRDFResource* source, 
      nsISimpleEnumerator** commands);

  NS_IMETHOD IsCommandEnabled(nsISupportsArray* aSources, 
      nsIRDFResource * aCommand, nsISupportsArray* aArguments, 
      PRBool* aResult);

  NS_IMETHOD DoCommand(nsISupportsArray* aSources, nsIRDFResource*   aCommand, 
      nsISupportsArray* aArguments);
};

//----------------------------------------------------------------------------
// Global functions and data [implementation]

NS_IMETHODIMP NS_NewCharsetMenu(nsISupports * aOuter, const nsIID & aIID, 
                                void ** aResult)
{
  if (!aResult) {
    return NS_ERROR_NULL_POINTER;
  }
  if (aOuter) {
    *aResult = nsnull;
    return NS_ERROR_NO_AGGREGATION;
  }
  nsCharsetMenu* inst = new nsCharsetMenu();
  if (!inst) {
    *aResult = nsnull;
    return NS_ERROR_OUT_OF_MEMORY;
  }
  nsresult res = inst->QueryInterface(aIID, aResult);
  if (NS_FAILED(res)) {
    *aResult = nsnull;
    delete inst;
  }
  return res;
}

//----------------------------------------------------------------------------
// Class nsCharsetMenu [implementation]

NS_IMPL_ISUPPORTS(nsCharsetMenu, nsIRDFDataSource::GetIID());

nsIRDFDataSource * nsCharsetMenu::mInner = NULL;
nsIRDFResource * nsCharsetMenu::kNC_BSCharsetMenuRoot = NULL;
nsIRDFResource * nsCharsetMenu::kNC_BDCharsetMenuRoot = NULL;
nsIRDFResource * nsCharsetMenu::kNC_BMCharsetMenuRoot = NULL;
nsIRDFResource * nsCharsetMenu::kNC_Name = NULL;

nsCharsetMenu::nsCharsetMenu() 
{
  NS_INIT_REFCNT();
  PR_AtomicIncrement(&g_InstanceCount);

  Init();
  CreateBrowserMenu();
}

nsCharsetMenu::~nsCharsetMenu() 
{
  Done();

  PR_AtomicDecrement(&g_InstanceCount);
}

nsresult nsCharsetMenu::Init() 
{
  nsresult res = NS_OK;
  nsIRDFService * rdfServ = NULL;
  nsIRDFContainerUtils * rdfUtil = NULL;

  res = nsServiceManager::GetService(kRDFServiceCID, kIRDFServiceIID, 
      (nsISupports **)&rdfServ);
  if (NS_FAILED(res)) goto done;

  rdfServ->GetResource(kURINC_BSCharsetMenuRoot, &kNC_BSCharsetMenuRoot);
  rdfServ->GetResource(kURINC_BDCharsetMenuRoot, &kNC_BDCharsetMenuRoot);
  rdfServ->GetResource(kURINC_BMCharsetMenuRoot, &kNC_BMCharsetMenuRoot);
  rdfServ->GetResource(kURINC_Name, &kNC_Name);

  res = nsComponentManager::CreateInstance(kRDFInMemoryDataSourceCID, nsnull, 
    kIRDFDataSourceIID, (void**) &mInner);
  if (NS_FAILED(res)) goto done;

  res = nsServiceManager::GetService(kRDFContainerUtilsCID, 
      kIRDFContainerUtilsIID, (nsISupports **)&rdfUtil);
  if (NS_FAILED(res)) goto done;

  res = rdfUtil->MakeSeq(mInner, kNC_BSCharsetMenuRoot, NULL);
  if (NS_FAILED(res)) goto done;
  res = rdfUtil->MakeSeq(mInner, kNC_BDCharsetMenuRoot, NULL);
  if (NS_FAILED(res)) goto done;
  res = rdfUtil->MakeSeq(mInner, kNC_BMCharsetMenuRoot, NULL);
  if (NS_FAILED(res)) goto done;

  res = rdfServ->RegisterDataSource(this, PR_FALSE);

done:
  if (rdfServ != NULL) nsServiceManager::ReleaseService(kRDFServiceCID, 
      rdfServ);
  if (rdfUtil != NULL) nsServiceManager::ReleaseService(kRDFContainerUtilsCID, 
      rdfUtil);

  return res;
}

nsresult nsCharsetMenu::Done() 
{
  nsresult res = NS_OK;
  nsIRDFService * rdfServ = NULL;

  res = nsServiceManager::GetService(kRDFServiceCID, kIRDFServiceIID, 
      (nsISupports **)&rdfServ);
  if (NS_FAILED(res)) goto done;

  res = rdfServ->UnregisterDataSource(this);

done:
  if (rdfServ != NULL) nsServiceManager::ReleaseService(kRDFServiceCID, 
      rdfServ);
  NS_IF_RELEASE(kNC_BSCharsetMenuRoot);
  NS_IF_RELEASE(kNC_BDCharsetMenuRoot);
  NS_IF_RELEASE(kNC_BMCharsetMenuRoot);
  NS_IF_RELEASE(kNC_Name);
  NS_IF_RELEASE(mInner);

  return res;
}

nsresult nsCharsetMenu::FillRDFContainer(nsIRDFService * aRDFServ,
                                         nsIRDFResource * aResource, 
                                         nsIStringBundle * aTitles,
                                         nsString ** aList, PRInt32 aCount) 
{
  nsresult res = NS_OK;
  nsCOMPtr<nsIRDFResource> node;
  nsCOMPtr<nsIRDFContainer> container;
  PRInt32 i;

  res = NewRDFContainer(mInner, aResource, getter_AddRefs(container));
  if (NS_FAILED(res)) return res;

  for (i = 0; i < aCount; i++) {
    nsString * cs = aList[i];
    if (cs == NULL) continue;

    // Make up a unique ID and create the RDF NODE
    char csID[256];
    cs->ToCString(csID, 256);
    res = aRDFServ->GetResource(csID, getter_AddRefs(node));
    if (NS_FAILED(res)) continue;

    nsAutoString csKey(cs->GetUnicode());
    csKey.Append(".title");

    PRUnichar * title = NULL;
    if (aTitles != NULL) 
      aTitles->GetStringFromName(csKey.GetUnicode(), &title);
    if (title == NULL) 
      title = (PRUnichar *)cs->GetUnicode();

    nsCOMPtr<nsIRDFLiteral> titleLiteral;
    res = aRDFServ->GetLiteral(title, getter_AddRefs(titleLiteral));
    if (NS_FAILED(res)) continue;
    res = Assert(node, kNC_Name, titleLiteral, PR_TRUE);
    if (NS_FAILED(res)) continue;

    // Add the element to the container
    res = container->AppendElement(node);
    if (NS_FAILED(res)) continue;
  }

  return res;
}

nsresult nsCharsetMenu::RemoveFlaggedCharsets(nsString ** aList, PRInt32 aCount,
                                              nsIStringBundle * aData, 
                                              char * aProp, PRBool value)
{
  nsresult res = NS_OK;

  for (PRInt32 i = 0; i < aCount; i++) {
    nsString * cs = aList[i];
    if (cs == NULL) continue;

    nsAutoString csKey(cs->GetUnicode());
    csKey.Append(aProp);

    PRUnichar * data = NULL;
    if (aData != NULL) 
      aData->GetStringFromName(csKey.GetUnicode(), &data);
    if (data == NULL) continue;

    aList[i] = NULL;
  }

  return res;
}

// XXX change "xuconv" to "uconv" when the new enc&dec trees are in place
nsresult nsCharsetMenu::CreateBrowserMenu() 
{
  nsresult res = NS_OK;
  nsCOMPtr<nsIStringBundle> titles = nsnull;
  nsCOMPtr<nsIStringBundle> data = nsnull;
  nsCOMPtr<nsILocale> locale = nsnull;

  NS_WITH_SERVICE(nsIRDFService, rdfServ, kRDFServiceCID, &res);
  if (NS_FAILED(res)) return res;

  NS_WITH_SERVICE(nsIStringBundleService, sbServ, kStringBundleServiceCID, &res);
  if (NS_FAILED(res)) return res;

  res = GetAppLocale(getter_AddRefs(locale));
  if (NS_FAILED(res)) return res;

  res = sbServ->CreateExtensibleBundle("software/netscape/intl/xuconv/titles/", locale, getter_AddRefs(titles));
  if (NS_FAILED(res)) return res;

  res = sbServ->CreateExtensibleBundle("software/netscape/intl/xuconv/data/", locale, getter_AddRefs(data));
  if (NS_FAILED(res)) return res;

  CreateBrowserMoreMenu(rdfServ, titles, data);
  CreateBrowserStaticMenu(rdfServ, titles, data);

  return NS_OK;
}

nsresult nsCharsetMenu::CreateBrowserMoreMenu(nsIRDFService * aRDFServ, 
                                              nsIStringBundle * aTitles, 
                                              nsIStringBundle * aData) 
{
  nsresult res = NS_OK;
  nsICharsetConverterManager * ccMan = NULL;
  nsString ** decs = NULL;
  PRInt32 count;

  res = nsServiceManager::GetService(kCharsetConverterManagerCID, 
      kICharsetConverterManagerIID, (nsISupports **)&ccMan);
  if (NS_FAILED(res)) goto done;

  res = ccMan->GetDecoderList(&decs, &count);
  if (NS_FAILED(res)) goto done;

  // put the flagged charsets on null
  RemoveFlaggedCharsets(decs, count, aData, ".notForBrowser", PR_TRUE);

  res = FillRDFContainer(aRDFServ, kNC_BMCharsetMenuRoot, aTitles, decs, count);
  if (NS_FAILED(res)) goto done;

done:
  if (ccMan != NULL) nsServiceManager::ReleaseService(
      kCharsetConverterManagerCID, ccMan);
  if (decs != NULL) delete [] decs;

  return res;
}

nsresult nsCharsetMenu::CreateBrowserStaticMenu(nsIRDFService * aRDFServ, 
                                                nsIStringBundle * aTitles, 
                                                nsIStringBundle * aData) 
{
  nsresult res = NS_OK;
  PRInt32 count = 2;
  nsString ** decs = new nsString * [count];
  decs[0] = new nsString("utf-8");
  decs[1] = new nsString("shift_jis");

  // note that we allow ALL the charsets to be placed here!

  res = FillRDFContainer(aRDFServ, kNC_BSCharsetMenuRoot, aTitles, decs, count);
  if (NS_FAILED(res)) goto done;

done:
  if (decs != NULL) {
    for (PRInt32 i = 0; i < count; i++) if (decs[i] != NULL) delete decs[i];
    delete [] decs;
  }

  return res;
}

nsresult nsCharsetMenu::NewRDFContainer(nsIRDFDataSource * aDataSource, 
                                        nsIRDFResource * aResource, 
                                        nsIRDFContainer ** aResult)
{
  nsresult res;

  res = nsComponentManager::CreateInstance(kRDFContainerCID, NULL, 
      nsIRDFContainer::GetIID(), (void**)aResult);
  if (NS_FAILED(res)) return res;

  res = (*aResult)->Init(aDataSource, aResource);
  if (NS_FAILED(res)) {
    NS_RELEASE(*aResult);
  }

  return res;
}

nsresult nsCharsetMenu::GetAppLocale(nsILocale ** aLocale)
{
  nsresult res = NS_OK;
  nsILocaleFactory * localeFactory;
  
  res = nsComponentManager::FindFactory(kLocaleFactoryCID, 
      (nsIFactory**)&localeFactory);
  if (NS_FAILED(res)) return res;

  res = localeFactory->GetApplicationLocale(aLocale);
  NS_RELEASE(localeFactory);
  return res;
}

//----------------------------------------------------------------------------
// Interface nsIRDFDataSource [implementation]

NS_IMETHODIMP nsCharsetMenu::GetURI(char ** uri)
{
  if (!uri) return NS_ERROR_NULL_POINTER;

  *uri = nsXPIDLCString::Copy("rdf:charset-menu");
  if (!(*uri)) return NS_ERROR_OUT_OF_MEMORY;

  return NS_OK;
}

NS_IMETHODIMP nsCharsetMenu::GetSource(nsIRDFResource* property,
                                       nsIRDFNode* target,
                                       PRBool tv,
                                       nsIRDFResource** source)
{
  return mInner->GetSource(property, target, tv, source);
}

NS_IMETHODIMP nsCharsetMenu::GetSources(nsIRDFResource* property,
                                        nsIRDFNode* target,
                                        PRBool tv,
                                        nsISimpleEnumerator** sources)
{
  return mInner->GetSources(property, target, tv, sources);
}

NS_IMETHODIMP nsCharsetMenu::GetTarget(nsIRDFResource* source,
                                       nsIRDFResource* property,
                                       PRBool tv,
                                       nsIRDFNode** target)
{
  return mInner->GetTarget(source, property, tv, target);
}

NS_IMETHODIMP nsCharsetMenu::GetTargets(nsIRDFResource* source,
                                        nsIRDFResource* property,
                                        PRBool tv,
                                        nsISimpleEnumerator** targets)
{
  return mInner->GetTargets(source, property, tv, targets);
}

NS_IMETHODIMP nsCharsetMenu::Assert(nsIRDFResource* aSource,
                                    nsIRDFResource* aProperty,
                                    nsIRDFNode* aTarget,
                                    PRBool aTruthValue)
{
  // TODO: filter out asserts we don't care about
  return mInner->Assert(aSource, aProperty, aTarget, aTruthValue);
}

NS_IMETHODIMP nsCharsetMenu::Unassert(nsIRDFResource* aSource,
                                      nsIRDFResource* aProperty,
                                      nsIRDFNode* aTarget)
{
  // TODO: filter out unasserts we don't care about
  return mInner->Unassert(aSource, aProperty, aTarget);
}


NS_IMETHODIMP nsCharsetMenu::Change(nsIRDFResource* aSource,
                                    nsIRDFResource* aProperty,
                                    nsIRDFNode* aOldTarget,
                                    nsIRDFNode* aNewTarget)
{
  // TODO: filter out changes we don't care about
  return mInner->Change(aSource, aProperty, aOldTarget, aNewTarget);
}

NS_IMETHODIMP nsCharsetMenu::Move(nsIRDFResource* aOldSource,
                                  nsIRDFResource* aNewSource,
                                  nsIRDFResource* aProperty,
                                  nsIRDFNode* aTarget)
{
  // TODO: filter out changes we don't care about
  return mInner->Move(aOldSource, aNewSource, aProperty, aTarget);
}


NS_IMETHODIMP nsCharsetMenu::HasAssertion(nsIRDFResource* source, 
                                          nsIRDFResource* property, 
                                          nsIRDFNode* target, PRBool tv, 
                                          PRBool* hasAssertion)
{
  return mInner->HasAssertion(source, property, target, tv, hasAssertion);
}

NS_IMETHODIMP nsCharsetMenu::AddObserver(nsIRDFObserver* n)
{
  return mInner->AddObserver(n);
}

NS_IMETHODIMP nsCharsetMenu::RemoveObserver(nsIRDFObserver* n)
{
  return mInner->RemoveObserver(n);
}

NS_IMETHODIMP nsCharsetMenu::ArcLabelsIn(nsIRDFNode* node, 
                                         nsISimpleEnumerator** labels)
{
  return mInner->ArcLabelsIn(node, labels);
}

NS_IMETHODIMP nsCharsetMenu::ArcLabelsOut(nsIRDFResource* source, 
                                          nsISimpleEnumerator** labels)
{
  return mInner->ArcLabelsOut(source, labels);
}

NS_IMETHODIMP nsCharsetMenu::GetAllResources(nsISimpleEnumerator** aCursor)
{
  return mInner->GetAllResources(aCursor);
}

NS_IMETHODIMP nsCharsetMenu::GetAllCommands(
                             nsIRDFResource* source,
                             nsIEnumerator/*<nsIRDFResource>*/** commands)
{
  NS_NOTYETIMPLEMENTED("write me!");
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP nsCharsetMenu::GetAllCmds(
                             nsIRDFResource* source,
                             nsISimpleEnumerator/*<nsIRDFResource>*/** commands)
{
  NS_NOTYETIMPLEMENTED("write me!");
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP nsCharsetMenu::IsCommandEnabled(
                             nsISupportsArray/*<nsIRDFResource>*/* aSources,
                             nsIRDFResource*   aCommand,
                             nsISupportsArray/*<nsIRDFResource>*/* aArguments,
                             PRBool* aResult)
{
  NS_NOTYETIMPLEMENTED("write me!");
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP nsCharsetMenu::DoCommand(nsISupportsArray* aSources,
                                       nsIRDFResource*   aCommand,
                                       nsISupportsArray* aArguments)
{
  NS_NOTYETIMPLEMENTED("write me!");
  return NS_ERROR_NOT_IMPLEMENTED;
}
