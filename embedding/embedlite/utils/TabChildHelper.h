/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __TabChildHelper_h_
#define __TabChildHelper_h_

#include "nsIObserver.h"
#include "FrameMetrics.h"
#include "nsFrameMessageManager.h"
#include "nsIWebNavigation.h"
#include "nsITabChild.h"
#include "nsIWidget.h"
#include "InputData.h"
#include "nsDataHashtable.h"
#include "nsIDOMEventListener.h"
#include "nsIDocument.h"
#include "TabChild.h"

class nsPresContext;
class nsIDOMWindowUtils;

namespace mozilla {

namespace layers {
struct ScrollableLayerGuid;
}

namespace embedlite {

class EmbedLiteViewChildIface;
class TabChildHelper : public mozilla::dom::TabChildBase,
                       public nsIDOMEventListener,
                       public nsITabChild,
                       public nsIObserver
{
public:
  typedef mozilla::layers::FrameMetrics::ViewID ViewID;
  TabChildHelper(EmbedLiteViewChildIface* aView);

  NS_DECL_ISUPPORTS_INHERITED
  NS_DECL_NSIDOMEVENTLISTENER
  NS_DECL_NSITABCHILD
  NS_DECL_NSIOBSERVER

  bool UpdateFrame(const mozilla::layers::FrameMetrics& aFrameMetrics);

  void DynamicToolbarMaxHeightChanged(const ScreenIntCoord &aHeight);
  virtual nsIWebNavigation* WebNavigation() const override;
  virtual nsIWidget* WebWidget() override;

  virtual bool DoLoadMessageManagerScript(const nsAString& aURL, bool aRunInGlobalScope) override;
  virtual bool DoSendBlockingMessage(JSContext* aCx,
                                     const nsAString& aMessage,
                                     mozilla::dom::ipc::StructuredCloneData& aData,
                                     JS::Handle<JSObject *> aCpows,
                                     nsIPrincipal* aPrincipal,
                                     nsTArray<mozilla::dom::ipc::StructuredCloneData>* aRetVal,
                                     bool aIsSync) override;
  virtual nsresult DoSendAsyncMessage(JSContext* aCx,
                                      const nsAString& aMessage,
                                      mozilla::dom::ipc::StructuredCloneData& aData,
                                      JS::Handle<JSObject *> aCpows,
                                      nsIPrincipal* aPrincipal) override;
  virtual bool DoUpdateZoomConstraints(const uint32_t& aPresShellId,
                                       const mozilla::layers::FrameMetrics::ViewID& aViewId,
                                       const Maybe<mozilla::layers::ZoomConstraints>& aConstraints) override;

  virtual ScreenIntSize GetInnerSize() override;

  void ReportSizeUpdate(const LayoutDeviceIntRect& aRect);

  mozilla::CSSPoint ApplyPointTransform(const LayoutDevicePoint& aPoint,
                                        const mozilla::layers::ScrollableLayerGuid& aGuid,
                                        bool *ok);

  void OpenIPC() { mIPCOpen = true; }

protected:
  virtual ~TabChildHelper();
  nsIWidget* GetWidget(nsPoint* aOffset);
  nsPresContext* GetPresContext();
  // Sends a simulated mouse event from a touch event for compatibility.
  bool ConvertMutiTouchInputToEvent(const mozilla::MultiTouchInput& aData,
                                    WidgetTouchEvent& aEvent);
  bool HasValidInnerSize();

private:
  bool InitTabChildGlobal();
  void Disconnect();
  void Unload();
  bool IPCOpen() const { return mIPCOpen; }

  friend class EmbedLiteViewThreadChild;
  friend class EmbedLiteViewProcessChild;
  friend class EmbedLiteViewChildIface;
  friend class EmbedLiteViewBaseChild;
  EmbedLiteViewChildIface* mView;
  bool mHasValidInnerSize;
  bool mIPCOpen;
  ScreenIntSize mInnerSize;
  ScreenIntCoord mDynamicToolbarMaxHeight;
};

}
}

#endif

