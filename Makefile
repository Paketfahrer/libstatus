export ARCHS = armv7 arm64 #armv6

export SYSROOT = $(THEOS)/sdks/iPhoneOS9.2.sdk
export SDKVERSION = 7.0
export TARGET = iphone:clang::7.0


export THEOS_DEVICE_IP=localhost

export THEOS_DEVICE_PORT=2222


TWEAK_NAME = libstatusbar
libstatusbar_FILES = libstatusbar.mm Classes.mm \
 							LSStatusBarClient.mm LSStatusBarServer.mm \
							UIStatusBarCustomItem.mm UIStatusBarCustomItemView.mm \
							LSStatusBarItem.mm #Testing.mm

#libstatusbar_FRAMEWORKS = UIKit

libstatusbar_PRIVATE_FRAMEWORKS = AppSupport SpringboardServices

libstatusbar_LDFLAGS += -lsubstrate -Wl,-segalign,4000

GO_EASY_ON_ME =1


before-stage::
	find . -name ".DS_Store" -delete

after-install::
	install.exec "killall -9 SpringBoard"

include $(THEOS)/makefiles/common.mk
include $(THEOS)/makefiles/tweak.mk

