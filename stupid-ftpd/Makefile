include $(TOPDIR)/rules.mk
	PKG_NAME:=stupid-ftpd
	PKG_RELEASE:=1
	PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
#	PKG_BUILD_DEPENDS:=libstdcpp

	include $(INCLUDE_DIR)/package.mk

define Package/stupid-ftpd
#	DEPENDS:=+libstdcpp
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Ftpserver for VX
endef

define Package/stupid-ftpd/description
	Ftpserver for Vip-Cam
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/stupid-ftpd/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/stupid-ftpd $(1)/bin/
endef
$(eval $(call BuildPackage,stupid-ftpd))
