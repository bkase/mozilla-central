/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsPluginArray_h___
#define nsPluginArray_h___

#include "nsCOMPtr.h"
#include "nsIDOMPluginArray.h"
#include "nsIDOMPlugin.h"
#include "nsIPluginHost.h"
#include "nsIURL.h"
#include "nsWeakReference.h"
#include "nsIObserver.h"

namespace mozilla {
namespace dom {
class Navigator;
} // namespace dom
} // namespace mozilla

class nsIDocShell;

// NB: Due to weak references, Navigator has intimate knowledge of our
// internals.
class nsPluginArray : public nsIDOMPluginArray
                    , public nsIObserver
                    , public nsSupportsWeakReference
{
public:
  nsPluginArray(mozilla::dom::Navigator* navigator, nsIDocShell *aDocShell);
  virtual ~nsPluginArray();

  NS_DECL_ISUPPORTS

  // nsIDOMPluginArray
  NS_DECL_NSIDOMPLUGINARRAY
  // nsIObserver
  NS_DECL_NSIOBSERVER

  // nsPluginArray registers itself as an observer with a weak reference.
  // This can't be done in the constructor, because at that point its
  // refcount is 0 (and it gets destroyed upon registration). So, Init()
  // must be called after construction.
  void Init();

  nsresult GetPluginHost(nsIPluginHost** aPluginHost);

  nsIDOMPlugin* GetItemAt(uint32_t aIndex, nsresult* aResult);
  nsIDOMPlugin* GetNamedItem(const nsAString& aName, nsresult* aResult);

  static nsPluginArray* FromSupports(nsISupports* aSupports)
  {
#ifdef DEBUG
    {
      nsCOMPtr<nsIDOMPluginArray> array_qi = do_QueryInterface(aSupports);

      // If this assertion fires the QI implementation for the object in
      // question doesn't use the nsIDOMPluginArray pointer as the nsISupports
      // pointer. That must be fixed, or we'll crash...
      NS_ASSERTION(array_qi == static_cast<nsIDOMPluginArray*>(aSupports),
                   "Uh, fix QI!");
    }
#endif

    return static_cast<nsPluginArray*>(static_cast<nsIDOMPluginArray*>(aSupports));
  }

private:
  nsresult GetPlugins();
  bool AllowPlugins();

public:
  void Invalidate();

protected:
  mozilla::dom::Navigator* mNavigator;
  nsCOMPtr<nsIPluginHost> mPluginHost;
  uint32_t mPluginCount;
  nsIDOMPlugin** mPluginArray;
  nsWeakPtr mDocShell;
};

class nsPluginElement : public nsIDOMPlugin
{
public:
  nsPluginElement(nsIDOMPlugin* plugin);
  virtual ~nsPluginElement();

  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMPLUGIN

  nsIDOMMimeType* GetItemAt(uint32_t aIndex, nsresult* aResult);
  nsIDOMMimeType* GetNamedItem(const nsAString& aName, nsresult* aResult);

  static nsPluginElement* FromSupports(nsISupports* aSupports)
  {
#ifdef DEBUG
    {
      nsCOMPtr<nsIDOMPlugin> plugin_qi = do_QueryInterface(aSupports);

      // If this assertion fires the QI implementation for the object in
      // question doesn't use the nsIDOMPlugin pointer as the nsISupports
      // pointer. That must be fixed, or we'll crash...
      NS_ASSERTION(plugin_qi == static_cast<nsIDOMPlugin*>(aSupports),
                   "Uh, fix QI!");
    }
#endif

    return static_cast<nsPluginElement*>(aSupports);
  }

private:
  nsresult GetMimeTypes();

protected:
  nsIDOMPlugin* mPlugin;
  uint32_t mMimeTypeCount;
  nsIDOMMimeType** mMimeTypeArray;
};

#endif /* nsPluginArray_h___ */
