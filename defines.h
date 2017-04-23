/*
 *  defines.h
 *  PandoraControls
 *
 *  Created by Public Nuisance on 7/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */


#ifdef TESTING
	#define NSLine() NSLog(@"%s %s %d", __FILE__, __FUNCTION__, __LINE__)
	#define HookLog(); \
		{ \
		uint32_t bt=0; \
		__asm__("mov %0, lr": "=r"(bt)); \
		NSLog(@"[%@ %s] bt=%x", [[self class] description], sel_getName(sel), bt); \
		}
	#define SelLog(); \
		{ \
		uint32_t bt=0; \
		__asm__("mov %0, lr": "=r"(bt)); \
		NSLog(@"%s bt=%x", __FUNCTION__, bt); \
		}
	#define NSType(obj) \
		NSLog(@"%@* " #obj  ";", [[obj class] description])
	#define NSDesc(obj) \
		NSLog(@"%@* " #obj  ";", [obj description])
	//#define NSRect(obj) \
	//	NSLog(@ #obj " = {{%f %f}{%f %f}}", obj.origin.x, obj.origin.y, obj.size.width, obj.size.height)
	#define CommonLog(fmt, ...) \
		{ \
			syslog(5, fmt, ##__VA_ARGS__); \
			fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
		}

	#define TRACE() \
	{ \
		NSArray* callStackReturnAddresses = [NSThread callStackReturnAddresses];	\
		int ncallStackReturnAddresses = [callStackReturnAddresses count];	\
		for(int level =0; level < ncallStackReturnAddresses; level++) \
		{	\
			NSNumber* addr = [callStackReturnAddresses objectAtIndex: level];	\
			uint32_t bt = [addr unsignedIntValue];	\
			Dl_info info; \
			dladdr((void*)bt, &info); \
			char* fname = strrchr(info.dli_fname, '/'); \
			if(fname) \
				fname++;\
			if(info.dli_sname) \
			{ \
				CommonLog("%d: %s %08x (%s + %08x)", level, fname, (uint32_t)bt - (uint32_t)info.dli_fbase, info.dli_sname, (uint32_t) bt - (uint32_t) info.dli_saddr); \
			} \
			else if(fname)\
			{ \
				CommonLog("%d: %s %08x (unknown)", level, fname, (uint32_t)bt - (uint32_t)info.dli_fbase); \
			} \
			else	\
			{	\
				CommonLog("%d: (unknown) %08x (unknown)", level, (uint32_t)bt); \
			}	\
		}	\
	}
#else
	#define NSLine()
	#define NSLog(...)
	#define HookLog();
	#define SelLog();
	#define NSType(...)
	#define NSDesc(...)
	//#define NSRect(...)
	#define CommonLog(...)
	#define TRACE()
#endif

	#define PROFILE(increment_var)	\
		for(timeval tim1 = (struct timeval){0}, tim2 = (struct timeval){0}; \
				!gettimeofday(&tim1, NULL) && !tim2.tv_sec; \
				gettimeofday(&tim2, NULL), increment_var += tim2.tv_usec - tim1.tv_usec + (tim2.tv_sec-tim1.tv_sec)*1000000)
	
	
	#define CommonLog_F(fmt, ...) \
		{ \
			syslog(5, fmt, ##__VA_ARGS__); \
			fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
		}

#if !defined(__arm64__)
	#define TRACE_F() \
	{ \
		NSArray* callStackReturnAddresses = [NSThread callStackReturnAddresses];	\
		int ncallStackReturnAddresses = [callStackReturnAddresses count];	\
		for(int level =0; level < ncallStackReturnAddresses; level++) \
		{	\
			NSNumber* addr = [callStackReturnAddresses objectAtIndex: level];	\
			uint32_t bt = [addr unsignedIntValue];	\
			Dl_info info; \
			dladdr((void*)bt, &info); \
			char* fname = strrchr(info.dli_fname, '/'); \
			if(fname) \
				fname++;\
			if(info.dli_sname) \
			{ \
				CommonLog_F("%d: %s %08x (%s + %08x)", level, fname, (uint32_t)bt - (uint32_t)info.dli_fbase, info.dli_sname, (uint32_t) bt - (uint32_t) info.dli_saddr); \
			} \
			else if(fname)\
			{ \
				CommonLog_F("%d: %s %08x (unknown)", level, fname, (uint32_t)bt - (uint32_t)info.dli_fbase); \
			} \
			else	\
			{	\
				CommonLog_F("%d: (unknown) %08x (unknown)", level, (uint32_t)bt); \
			}	\
		}	\
	}
#else
	#define TRACE_F() \
	{ \
		NSArray* callStackReturnAddresses = [NSThread callStackReturnAddresses];	\
		int ncallStackReturnAddresses = [callStackReturnAddresses count];	\
		for(int level =0; level < ncallStackReturnAddresses; level++) \
		{	\
			NSNumber* addr = [callStackReturnAddresses objectAtIndex: level];	\
			uint64_t bt = [addr unsignedLongValue];	\
			Dl_info info; \
			dladdr((void*)bt, &info); \
			char* fname = strrchr(info.dli_fname, '/'); \
			if(fname) \
				fname++;\
			if(info.dli_sname) \
			{ \
				CommonLog_F("%d: %s %016llx (%s + %016llx)", level, fname, (uint64_t)bt - (uint64_t)info.dli_fbase, info.dli_sname, (uint64_t) bt - (uint64_t) info.dli_saddr); \
			} \
			else if(fname)\
			{ \
				CommonLog_F("%d: %s %016llx (unknown)", level, fname, (uint64_t)bt - (uint64_t)info.dli_fbase); \
			} \
			else	\
			{	\
				CommonLog_F("%d: (unknown) %016llx (unknown)", level, (uint64_t)bt); \
			}	\
		}	\
	}

#endif


#define HOOKDEF(type, class, name, args...) \
static type (*_ ## class ## $ ## name)(class *self, SEL sel, ## args); \
static type $ ## class ## $ ## name(class *self, SEL sel, ## args)

#define HOOK(type, class, name, args...) \
static type $ ## class ## $ ## name(class *self, SEL sel, ## args)

#define CALL_ORIG(class, name, args...) \
_ ## class ## $ ## name(self, sel, ## args)

#define GETCLASS(class) \
Class $ ## class  = objc_getClass(#class)


#define HOOKMESSAGE(class, sel, selnew) \
MSHookMessageEx( $ ## class, @selector(sel), (IMP)$ ## class ## $ ## selnew, (IMP*)&_ ## class ## $ ## selnew);
//_ ## class ## $ ## selnew = MSHookMessage( $ ## class, @selector(sel), &$ ## class ## $ ## selnew);

#define HOOKCLASSMESSAGE(class, sel, selnew) \
MSHookMessageEx( object_getClass($ ## class), @selector(sel), (IMP)$ ## class ## $ ## selnew, (IMP*)&_ ## class ## $ ## selnew);
//_ ## class ## $ ## selnew = MSHookMessage( object_getClass($ ## class), @selector(sel), &$ ## class ## $ ## selnew);

#define IVGETVAR(type, name); \
static type name; \
Ivar IV$ ## name = object_getInstanceVariable(self, #name, reinterpret_cast<void **> (& name));

#define GETVAR(type, name); \
static type name; \
object_getInstanceVariable(self, #name, reinterpret_cast<void **> (& name));

#define GETIVAR(type, name) \
name = (type) object_getIvar(self, IV$ ## name)

#define SETIVAR(name) \
object_setIvar(self, IV$ ## name, (id) name)

#define SETVAL(name, value); \
name = value; \
object_setIvar(self, IV$ ## name, (id) name);

#define CLASSALLOC(class) \
[objc_getClass(#class) alloc]



#ifndef NO_PROTECTION
	#define FULL_PROTECTION
#endif


#ifndef kCFCoreFoundationVersionNumber_iOS_3_0
#define kCFCoreFoundationVersionNumber_iOS_3_0 478.47
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_3_1
#define kCFCoreFoundationVersionNumber_iOS_3_1 478.52
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_3_2
#define kCFCoreFoundationVersionNumber_iOS_3_2 478.61
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_4_0
#define kCFCoreFoundationVersionNumber_iOS_4_0 550.32
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_4_1
#define kCFCoreFoundationVersionNumber_iOS_4_1 550.38
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_4_2
#define kCFCoreFoundationVersionNumber_iOS_4_2 550.52
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_4_3
#define kCFCoreFoundationVersionNumber_iOS_4_3 550.58
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_5_0
#define kCFCoreFoundationVersionNumber_iOS_5_0 675.00
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_5_1
#define kCFCoreFoundationVersionNumber_iOS_5_1 690.10
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_6_0
#define kCFCoreFoundationVersionNumber_iOS_6_0 793.00
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_6_1
#define kCFCoreFoundationVersionNumber_iOS_6_1 793.00
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_7_0
#define kCFCoreFoundationVersionNumber_iOS_7_0 847.20
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_7_0_3
#define kCFCoreFoundationVersionNumber_iOS_7_0_3 847.21
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_7_1_0
#define kCFCoreFoundationVersionNumber_iOS_7_1_0 847.24
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_7_1
#define kCFCoreFoundationVersionNumber_iOS_7_1 847.26
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_7_1_2
#define kCFCoreFoundationVersionNumber_iOS_7_1_2 847.27
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_8_0
#define kCFCoreFoundationVersionNumber_iOS_8_0 1140.10
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_8_1
#define kCFCoreFoundationVersionNumber_iOS_8_1 1141.14
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_8_2
#define kCFCoreFoundationVersionNumber_iOS_8_2 1142.16
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_8_3
#define kCFCoreFoundationVersionNumber_iOS_8_3 1144.17
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_8_4
#define kCFCoreFoundationVersionNumber_iOS_8_4 1145.15
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_9_0
#define kCFCoreFoundationVersionNumber_iOS_9_0 1240.1
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_9_1
#define kCFCoreFoundationVersionNumber_iOS_9_1 1241.11
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_9_2
#define kCFCoreFoundationVersionNumber_iOS_9_2 1242.13
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_9_3
#define kCFCoreFoundationVersionNumber_iOS_9_3 1280.30
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_9_3_3
#define kCFCoreFoundationVersionNumber_iOS_9_3_3 1290.11
#endif

#ifndef kCFCoreFoundationVersionNumber10_10
#define kCFCoreFoundationVersionNumber10_10 1151.16
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_10_1_1 
#define kCFCoreFoundationVersionNumber_iOS_10_1_1 1348.000000
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_10_2 
#define kCFCoreFoundationVersionNumber_iOS_10_2 1348.220000
#endif


#define IS_IOS_OR_OLDER(version) (kCFCoreFoundationVersionNumber <= kCFCoreFoundationVersionNumber_##version)
#define IS_IOS_OR_NEWER(version) (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_##version)
#define IS_IOS_BETWEEN(start, end) (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_##start && kCFCoreFoundationVersionNumber <= kCFCoreFoundationVersionNumber_##end)

