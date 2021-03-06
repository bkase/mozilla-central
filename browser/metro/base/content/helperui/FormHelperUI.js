/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * Responsible for filling in form information.
 *  - displays select popups
 *  - Provides autocomplete box for input fields.
 */

const kBrowserFormZoomLevelMin = 0.8;
const kBrowserFormZoomLevelMax = 2.0;

var FormHelperUI = {
  _debugEvents: false,
  _currentBrowser: null,
  _currentElement: null,
  _currentCaretRect: null,
  _currentElementRect: null,
  _open: false,

  type: "form",

  init: function formHelperInit() {
    // Listen for form assistant messages from content
    messageManager.addMessageListener("FormAssist:Show", this);
    messageManager.addMessageListener("FormAssist:Hide", this);
    messageManager.addMessageListener("FormAssist:Update", this);
    messageManager.addMessageListener("FormAssist:AutoComplete", this);

    // Listen for events where form assistant should be closed or updated
    let tabs = Elements.tabList;
    tabs.addEventListener("TabSelect", this, true);
    tabs.addEventListener("TabClose", this, true);
    Elements.browsers.addEventListener("URLChanged", this, true);
    Elements.browsers.addEventListener("SizeChanged", this, true);

    // Listen some events to show/hide arrows
    Elements.browsers.addEventListener("PanBegin", this, false);
    Elements.browsers.addEventListener("PanFinished", this, false);
  },

  /*
   * Open of the form helper proper. Currently doesn't display anything
   * on metro since the nav buttons are off.
   */
  show: function formHelperShow(aElement) {
    // Delay the operation until all resize operations generated by the
    // keyboard apparition are done.
    if (!InputSourceHelper.isPrecise && aElement.editable &&
        ContentAreaObserver.isKeyboardTransitioning) {
      this._waitForKeyboard(aElement);
      return;
    }

    this._currentBrowser = Browser.selectedBrowser;
    this._currentCaretRect = null;

    let lastElement = this._currentElement || null;

    this._currentElement = {
      id: aElement.id,
      name: aElement.name,
      title: aElement.title,
      value: aElement.value,
      maxLength: aElement.maxLength,
      type: aElement.type,
      choices: aElement.choices,
      isAutocomplete: aElement.isAutocomplete,
      list: aElement.list,
      rect: aElement.rect
    };

    this._updateContainerForSelect(lastElement, this._currentElement);
    this._updatePopupsFor(this._currentElement);

    // Prevent the view to scroll automatically while typing
    this._currentBrowser.scrollSync = false;

    this._open = true;
  },

  hide: function formHelperHide() {
    this._open = false;

    SelectHelperUI.hide();
    AutofillMenuUI.hide();

    // Restore the scroll synchonisation
    if (this._currentBrowser)
      this._currentBrowser.scrollSync = true;

    // reset current Element and Caret Rect
    this._currentElementRect = null;
    this._currentCaretRect = null;

    this._updateContainerForSelect(this._currentElement, null);
    if (this._currentBrowser)
      this._currentBrowser.messageManager.sendAsyncMessage("FormAssist:Closed", { });
  },

  _onShowRequest: function _onShowRequest(aJsonMsg) {
    if (aJsonMsg.current.choices) {
      // Note, aJsonMsg.current.rect is translated from browser to client
      // in the SelectHelperUI code.
      SelectHelperUI.show(aJsonMsg.current.choices, aJsonMsg.current.title,
                          aJsonMsg.current.rect);
    } else {
      this._currentBrowser = getBrowser();
      this._currentElementRect =
        Rect.fromRect(this._currentBrowser.rectBrowserToClient(aJsonMsg.current.rect));
      this.show(aJsonMsg.current);
    }
  },

  /*
   * Events
   */

  handleEvent: function formHelperHandleEvent(aEvent) {
    if (this._debugEvents) Util.dumpLn(aEvent.type);

    if (!this._open)
      return;

    switch (aEvent.type) {
      case "TabSelect":
      case "TabClose":
      case "PanBegin":
      case "SizeChanged":
        this.hide();
        break;

      case "URLChanged":
        if (aEvent.detail && aEvent.target == getBrowser())
          this.hide();
        break;
    }
  },

  receiveMessage: function formHelperReceiveMessage(aMessage) {
    if (this._debugEvents) Util.dumpLn(aMessage.name);
    let json = aMessage.json;

    switch (aMessage.name) {
      case "FormAssist:Show":
        this._onShowRequest(json);
        break;

      case "FormAssist:Hide":
        this.hide();
        break;

      case "FormAssist:AutoComplete":
        this.show(json.current);
        break;
    }
  },

  doAutoComplete: function formHelperDoAutoComplete(aData) {
    this._currentBrowser.messageManager.sendAsyncMessage("FormAssist:AutoComplete",
      { value: aData });
  },

  _updatePopupsFor: function _formHelperUpdatePopupsFor(aElement) {
    if (!this._updateSuggestionsFor(aElement)) {
      AutofillMenuUI.hide();
    }
  },

  /*
   * Populates the autofill menu for this element.
   */
  _updateSuggestionsFor: function _formHelperUpdateSuggestionsFor(aElement) {
    let suggestions = this._getAutocompleteSuggestions(aElement);
    if (!suggestions.length)
      return false;
    AutofillMenuUI.show(this._currentElementRect, suggestions);
    return true;
  },

  /*
   * Retrieve the autocomplete list from the autocomplete service for an element
   */
  _getAutocompleteSuggestions: function _formHelperGetAutocompleteSuggestions(aElement) {
    if (!aElement.isAutocomplete) {
      return [];
    }

    let suggestions = [];

    let autocompleteService = Cc["@mozilla.org/satchel/form-autocomplete;1"].getService(Ci.nsIFormAutoComplete);
    let results = autocompleteService.autoCompleteSearch(aElement.name || aElement.id, aElement.value, aElement, null);
    if (results.matchCount > 0) {
      for (let i = 0; i < results.matchCount; i++) {
        let value = results.getValueAt(i);

        // Do not show the value if it is the current one in the input field
        if (value == aElement.value)
          continue;

        suggestions.push({ "label": value, "value": value});
      }
    }

    // Add the datalist elements provided by the website, note that the
    // displayed value can differ from the real value of the element.
    let options = aElement.list;
    for (let i = 0; i < options.length; i++)
      suggestions.push(options[i]);

    return suggestions;
  },

  /*
   * Setup for displaying the selection choices menu
   */
  _updateContainerForSelect: function _formHelperUpdateContainerForSelect(aLastElement, aCurrentElement) {
    let lastHasChoices = aLastElement && (aLastElement.choices != null);
    let currentHasChoices = aCurrentElement && (aCurrentElement.choices != null);

    if (currentHasChoices)
      SelectHelperUI.show(aCurrentElement.choices, aCurrentElement.title, aCurrentElement.rect);
    else if (lastHasChoices)
      SelectHelperUI.hide();
  },

  _waitForKeyboard: function formHelperWaitForKeyboard(aElement) {
    let self = this;
    window.addEventListener("KeyboardChanged", function(aEvent) {
      window.removeEventListener("KeyboardChanged", arguments.callee, false);
      self._currentBrowser.messageManager.sendAsyncMessage("FormAssist:Update", {});
    }, false);
  }
};

