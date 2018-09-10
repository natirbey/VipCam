include $(TOPDIR)/rules.mk
	PKG_NAME:=openRTSP-mini
	PKG_RELEASE:=1
	PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
	PKG_BUILD_DEPENDS:=libstdcpp

	include $(INCLUDE_DIR)/package.mk

define Package/openRTSP-mini
	DEPENDS:=+libstdcpp
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Openrtsp for ipcamera
endef

define Package/openRTSP-mini/description
	Openrtsp for ipcamera
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./or/* $(PKG_BUILD_DIR)/
endef

define Package/openRTSP-mini/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/testProgs/openRTSP $(1)/bin/
endef
$(eval $(call BuildPackage,openRTSP-mini,+libstdcpp))
