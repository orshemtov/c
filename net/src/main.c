#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <stdio.h>

typedef struct
{
    char *data;
    size_t size;
} MemoryStruct;

typedef struct
{
    char full_name[256];
    char description[512];
    char language[64];
    char html_url[256];
    char created_at[32];
    char updated_at[32];
    char pushed_at[32];
    char license_name[64];
    long stargazers_count;
    long forks_count;
    long open_issues_count;
    long watchers_count;
} RepoInfo;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;

    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if (ptr == NULL)
    {
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

char *http_get(const char *url)
{
    CURL *curl;
    CURLcode res;
    MemoryStruct chunk = {0};
    long status;

    curl = curl_easy_init();
    if (!curl)
        return NULL;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "User-Agent: CustomClient/1.0");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);

    if (status != 200)
    {
        fprintf(stderr, "HTTP request failed with status: %ld\n", status);
        free(chunk.data);
        chunk.data = NULL;
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK)
    {
        free(chunk.data);
        return NULL;
    }

    return chunk.data;
}

int parse_repo_info(const char *json, RepoInfo *out)
{
    if (!json || !out)
        return 0;

    cJSON *root = cJSON_Parse(json);
    if (!root)
        return 0;

    cJSON *full_name = cJSON_GetObjectItemCaseSensitive(root, "full_name");
    cJSON *description = cJSON_GetObjectItemCaseSensitive(root, "description");
    cJSON *language = cJSON_GetObjectItemCaseSensitive(root, "language");
    cJSON *html_url = cJSON_GetObjectItemCaseSensitive(root, "html_url");
    cJSON *created_at = cJSON_GetObjectItemCaseSensitive(root, "created_at");
    cJSON *updated_at = cJSON_GetObjectItemCaseSensitive(root, "updated_at");
    cJSON *pushed_at = cJSON_GetObjectItemCaseSensitive(root, "pushed_at");
    cJSON *license = cJSON_GetObjectItemCaseSensitive(root, "license");
    cJSON *license_name = license ? cJSON_GetObjectItemCaseSensitive(license, "name") : NULL;
    cJSON *stargazers_count = cJSON_GetObjectItemCaseSensitive(root, "stargazers_count");
    cJSON *forks_count = cJSON_GetObjectItemCaseSensitive(root, "forks_count");
    cJSON *open_issues_count = cJSON_GetObjectItemCaseSensitive(root, "open_issues_count");
    cJSON *watchers_count = cJSON_GetObjectItemCaseSensitive(root, "watchers_count");

    snprintf(out->full_name, sizeof(out->full_name), "%s", cJSON_IsString(full_name) ? full_name->valuestring : "N/A");
    snprintf(out->description, sizeof(out->description), "%s", cJSON_IsString(description) ? description->valuestring : "N/A");
    snprintf(out->language, sizeof(out->language), "%s", cJSON_IsString(language) ? language->valuestring : "N/A");
    snprintf(out->html_url, sizeof(out->html_url), "%s", cJSON_IsString(html_url) ? html_url->valuestring : "N/A");
    snprintf(out->created_at, sizeof(out->created_at), "%s", cJSON_IsString(created_at) ? created_at->valuestring : "N/A");
    snprintf(out->updated_at, sizeof(out->updated_at), "%s", cJSON_IsString(updated_at) ? updated_at->valuestring : "N/A");
    snprintf(out->pushed_at, sizeof(out->pushed_at), "%s", cJSON_IsString(pushed_at) ? pushed_at->valuestring : "N/A");
    snprintf(out->license_name, sizeof(out->license_name), "%s", cJSON_IsString(license_name) ? license_name->valuestring : "N/A");

    out->stargazers_count = cJSON_IsNumber(stargazers_count) ? stargazers_count->valueint : 0;
    out->forks_count = cJSON_IsNumber(forks_count) ? forks_count->valueint : 0;
    out->open_issues_count = cJSON_IsNumber(open_issues_count) ? open_issues_count->valueint : 0;
    out->watchers_count = cJSON_IsNumber(watchers_count) ? watchers_count->valueint : 0;

    cJSON_Delete(root);

    return 1;
}

int main()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    char *body = http_get("https://api.github.com/repos/torvalds/linux");
    if (!body)
    {
        fprintf(stderr, "HTTP GET request failed\n");
        curl_global_cleanup();
        return 1;
    }

    RepoInfo r = {0};
    if (parse_repo_info(body, &r))
    {
        printf("Repo: %s\nStars: %ld\nLast push: %s\n",
               r.full_name, r.stargazers_count, r.pushed_at);
    }
    else
    {
        fprintf(stderr, "Failed to parse JSON\n");
    }

    free(body);

    curl_global_cleanup();

    return 0;
}