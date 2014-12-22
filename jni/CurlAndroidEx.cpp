#include <jni.h>
#include <stdio.h>
#include <curl/curl.h>
#include <android/log.h>
#include <stdlib.h>

typedef struct MemoryStruct {
	char *memory;
	size_t size;
} MemoryStruct;

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
		void *userp) {
	size_t realsize = size * nmemb;
	MemoryStruct *mem = (MemoryStruct *) userp;

	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

jstring loadPage(JNIEnv *env, jobject jobj, jstring url) {
	MemoryStruct page;
	CURL *curl;
	CURLcode res;
	jboolean *iscopy;
	char *buffer;
	const char *webpage;
	webpage = (env)->GetStringUTFChars(url, NULL);
	if (webpage == NULL) {
		return NULL; /* OutOfMemoryError already thrown */
	}

	page.memory = (char *)malloc(1); /* will be grown as needed by the realloc above */
	page.size = 0;

	buffer = (char *) malloc(1024);

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	__android_log_print(ANDROID_LOG_INFO, "CurlAndroidEx C Code", "MyURL %s",
			webpage);
	if (curl) {
		__android_log_print(ANDROID_LOG_INFO, "CurlAndroidEx C Code",
				"Inside Curl Block %s", curl);
		curl_easy_setopt(curl, CURLOPT_URL, webpage);
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &page);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);

//		curl_easy_setopt(curl, CURLOPT_URL, myurl);

		/* always cleanup */
		curl_easy_cleanup(curl);

		if (page.memory)
			free(page.memory);

		/* we're done with libcurl, so clean it up */
		curl_global_cleanup();

		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		else {
			sprintf(buffer, "curl_easy_perform() success: datalen%d src\n %s\n",
					page.size, page.memory);

			return env->NewStringUTF(buffer);
		}
	}
	return env->NewStringUTF("No Data");;
}

typedef struct pageInfo_t {
	char *data;
	int len;
} pageInfo_t;

static size_t HTTPData(void *buffer, size_t size, size_t nmemb,
		void *userData) {
	int len = size * nmemb;
	pageInfo_t *page = (pageInfo_t *) userData;

	if (buffer && page->data && (page->len + len < (16 * 1024))) {
		memcpy(&page->data[page->len], buffer, len);
		page->len += len;
	}
	return len;
}

jstring loadPagepageInfo_t(JNIEnv *env, jobject jobj, jstring url) {
	pageInfo_t page;
	CURL *curl;
	CURLcode res;
	jboolean *iscopy;
	char *buffer;
	const char *webpage;
	webpage = (env)->GetStringUTFChars(url, NULL);
	if (webpage == NULL) {
		return NULL; /* OutOfMemoryError already thrown */
	}

	page.data = (char *) malloc(16 * 1024);
	page.len = 0;
	if (page.data)
		memset(page.data, 32, 16 * 1024);

	buffer = (char *) malloc(1024);

	curl = curl_easy_init();

	__android_log_print(ANDROID_LOG_INFO, "CurlAndroidEx C Code", "MyURL %s",
			webpage);
	if (curl) {
		__android_log_print(ANDROID_LOG_INFO, "CurlAndroidEx C Code",
				"Inside Curl Block %s", curl);
		curl_easy_setopt(curl, CURLOPT_URL, webpage);
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HTTPData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &page);
		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);

//		curl_easy_setopt(curl, CURLOPT_URL, myurl);

		/* always cleanup */
		curl_easy_cleanup(curl);

		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		else {
			page.data[page.len < 256 ? page.len : 256] = '\0';
			sprintf(buffer, "curl_easy_perform() success: datalen%d src\n %s\n",
					page.len, page.data);

			return env->NewStringUTF(buffer);
		}
	}
	return env->NewStringUTF("No Data");;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* pVm, void* reserved) {
	JNIEnv* env;
	if ((pVm)->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
		return -1;
	}
	JNINativeMethod nm[1];
	nm[0].name = "loadPage";
	nm[0].signature = "(Ljava/lang/String;)Ljava/lang/String;";
	nm[0].fnPtr = (void*) loadPage;

	jclass cls = (env)->FindClass("com/dastan/curlandroidex/MainActivity");
	// Register methods with env->RegisterNatives.
	(env)->RegisterNatives(cls, nm, 1);
	return JNI_VERSION_1_6;
}
