/* udc - url data cache - a caching system that keeps blocks of data fetched from URLs in
 * sparse local files for quick use the next time the data is needed.
 *
 * This cache is enormously simplified by there being no local _write_ to the cache,
 * just reads.
 *
 * The overall strategy of the implementation is to have a root cache directory
 * with a subdir for each file being cached.  The directory for a single cached file
 * contains two files - "bitmap" and "sparseData" that contains information on which
 * parts of the URL are cached and the actual cached data respectively. The subdirectory name
 * associated with the file is constructed from the URL in a straightforward manner.
 *     http://genome.ucsc.edu/cgi-bin/hgGateway
 * gets mapped to:
 *     rootCacheDir/http/genome.ucsc.edu/cgi-bin/hgGateway/
 * The URL protocol is the first directory under the root, and the remainder of the
 * URL, with some necessary escaping, is used to define the rest of the cache directory
 * structure, with each '/' after the protocol line translating into another directory
 * level.
 *
 * The bitmap file contains time stamp and size data as well as an array with one bit
 * for each block of the file that has been fetched.  Currently the block size is 8K.
 *
 * This is a port of the UCSC browser code to speed up random access using HTTP/1.1.  It
 * also was designed to not require including common.h, as that leads to name conflicts.
 */

#ifndef GB_UDC_H
#define GB_UDC_H
#ifdef __cplusplus
extern "C" {
#endif

/* this check allows file to compile without common.h */
#ifndef COMMON_H
#define bits64 unsigned long long  /* Wants to be unsigned 64 bits. */
#define bits32 unsigned       /* Wants to be unsigned 32 bits. */
#define bits16 unsigned short /* Wants to be unsigned 16 bits. */
#define boolean int
#endif

struct gb_udcFile;
/* Handle to a cached file.  Inside of structure mysterious unless you are gb_udc.c. */

struct gb_udcFile *gb_udcFileMayOpen(char *url, char *cacheDir, bits32 blockSize);
/* Open up a cached file. cacheDir may be null in which case gb_udcDefaultDir() will be
 * used.  Return NULL if file doesn't exist.
 * If blockSize is zero, the default is used. Should be a power of two.
 */

struct gb_udcFile *gb_udcFileOpen(char *url, char *cacheDir, bits32 blockSize);
/* Open up a cached file.  cacheDir may be null in which case gb_udcDefaultDir() will be
 * used.  Abort if if file doesn't exist.
 * If blockSize is zero, the default is used. Should be a power of two.
 */

void gb_udcFileClose(struct gb_udcFile **pFile);
/* Close down cached file. */

bits64 gb_udcRead(struct gb_udcFile *file, void *buf, bits64 size);
/* Read a block from file.  Return amount actually read. */

#define gb_udcReadOne(file, var) gb_udcRead(file, &(var), sizeof(var))
/* Read one variable from file or die. */

void gb_udcMustRead(struct gb_udcFile *file, void *buf, bits64 size);
/* Read a block from file.  Abort if any problem, including EOF before size is read. */

#define gb_udcMustReadOne(file, var) gb_udcMustRead(file, &(var), sizeof(var))
/* Read one variable from file or die. */

bits64 gb_udcReadBits64(struct gb_udcFile *file, boolean isSwapped);
/* Read and optionally byte-swap 64 bit entity. */

bits32 gb_udcReadBits32(struct gb_udcFile *file, boolean isSwapped);
/* Read and optionally byte-swap 32 bit entity. */

bits16 gb_udcReadBits16(struct gb_udcFile *file, boolean isSwapped);
/* Read and optionally byte-swap 16 bit entity. */

float gb_udcReadFloat(struct gb_udcFile *file, boolean isSwapped);
/* Read and optionally byte-swap floating point number. */

double gb_udcReadDouble(struct gb_udcFile *file, boolean isSwapped);
/* Read and optionally byte-swap double-precision floating point number. */

int gb_udcGetChar(struct gb_udcFile *file);
/* Get next character from file or die trying. */

char *gb_udcReadLine(struct gb_udcFile *file);
/* Fetch next line from gb_udc cache. */

char *gb_udcReadStringAndZero(struct gb_udcFile *file);
/* Read in zero terminated string from file.  Do a freeMem of result when done. */

char *gb_udcFileReadAll(char *url, char *cacheDir, size_t maxSize, size_t *retSize);
/* Read a complete file via UDC. The cacheDir may be null in which case gb_udcDefaultDir()
 * will be used.  If maxSize is non-zero, check size against maxSize
 * and abort if it's bigger.  Returns file data (with an extra terminal for the
 * common case where it's treated as a C string).  If retSize is non-NULL then
 * returns size of file in *retSize. Do a freeMem or freez of the returned buffer
 * when done. */

#if 0  // drop for HAL
struct lineFile *gb_udcWrapShortLineFile(char *url, char *cacheDir, size_t maxSize);
/* Read in entire short (up to maxSize) url into memory and wrap a line file around it.
 * The cacheDir may be null in which case gb_udcDefaultDir() will be used.  If maxSize
 * is zero then a default value (currently 64 meg) will be used. */
#endif

void gb_udcSeek(struct gb_udcFile *file, bits64 offset);
/* Seek to a particular (absolute) position in file. */

void gb_udcSeekCur(struct gb_udcFile *file, bits64 offset);
/* Seek to a particular (from current) position in file. */

bits64 gb_udcTell(struct gb_udcFile *file);
/* Return current file position. */

bits64 gb_udcCleanup(char *cacheDir, double maxDays, boolean testOnly);
/* Remove cached files older than maxDays old. If testOnly is set
 * no clean up is done, but the size of the files that would be
 * cleaned up is still. */

void gb_udcParseUrl(char *url, char **retProtocol, char **retAfterProtocol, char **retColon);
/* Parse the URL into components that udc treats separately.
 * *retAfterProtocol is Q-encoded to keep special chars out of filenames.
 * Free all *ret's except *retColon when done. */

void gb_udcParseUrlFull(char *url, char **retProtocol, char **retAfterProtocol, char **retColon, char **retAuth);
/* Parse the URL into components that udc treats separately.
 * *retAfterProtocol is Q-encoded to keep special chars out of filenames.
 * Free all *ret's except *retColon when done. */

char *gb_udcDefaultDir();
/* Get default directory for cache.  Use this for the gb_udcFileOpen call if you
 * don't have anything better.... */

void gb_udcSetDefaultDir(char *path);
/* Set default directory for cache */

void gb_udcDisableCache();
/* Switch off caching. Re-enable with gb_udcSetDefaultDir */

#define gb_udcDevicePrefix "udc:"
/* Prefix used by convention to indicate a file should be accessed via udc.  This is
 * followed by the local path name or a url, so in common practice you see things like:
 *     udc:http://genome.ucsc.edu/goldenPath/hg18/tracks/someTrack.bb */

struct slName *gb_udcFileCacheFiles(char *url, char *cacheDir);
/* Return low-level list of files used in cache. */

char *gb_udcPathToUrl(const char *path, char *buf, size_t size, char *cacheDir);
/* Translate path into an URL, store in buf, return pointer to buf if successful
 * and NULL if not. */

long long int gb_udcSizeFromCache(char *url, char *cacheDir);
/* Look up the file size from the local cache bitmap file, or -1 if there
 * is no cache for url. */

time_t gb_udcTimeFromCache(char *url, char *cacheDir);
/* Look up the file datetime from the local cache bitmap file, or 0 if there
 * is no cache for url. */

unsigned long gb_udcCacheAge(char *url, char *cacheDir);
/* Return the age in seconds of the oldest cache file.  If a cache file is
 * missing, return the current time (seconds since the epoch). */

int gb_udcCacheTimeout();
/* Get cache timeout (if local cache files are newer than this many seconds,
 * we won't ping the remote server to check the file size and update time). */

void gb_udcSetCacheTimeout(int timeout);
/* Set cache timeout (if local cache files are newer than this many seconds,
 * we won't ping the remote server to check the file size and update time). */

time_t gb_udcUpdateTime(struct gb_udcFile *udc);
/* return udc->updateTime */

boolean gb_udcFastReadString(struct gb_udcFile *f, char buf[256]);
/* Read a string into buffer, which must be long enough
 * to hold it.  String is in 'writeString' format. */

off_t gb_udcFileSize(char *url);
/* fetch remote or local file size from given URL or path */

boolean gb_udcExists(char *url);
/* return true if a remote or local file exists */

boolean gb_udcIsLocal(char *url);
/* return true if url is not a http or ftp file, just a normal local file path */

void gb_udcSetLog(FILE *fp);
/* Tell GB_UDC where to log its statistics. */

void gb_udcMMap(struct gb_udcFile *file);
/* Enable access to underlying file as memory using mmap.  gb_udcMMapFetch
 * must be called to actually access regions of the file. */

void *gb_udcMMapFetch(struct gb_udcFile *file, bits64 offset, bits64 size);
/* Return pointer to a region of the file in memory, ensuring that regions is
 * cached.. gb_udcMMap must have been called to enable access.  This must be
 * called for first access to a range of the file or erroneous (zeros) data
 * maybe returned.  Maybe called multiple times on a range or overlapping
 * returns. */

    
/* below are added to avoid comflicts with including common.h */
void gb_udcVerboseSetLevel(int l);
/* set the verbose level; */

void gb_udcFreeMem(void *mem);
/* free memory allocated by gb_udc functions */
    
#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif /* GB_UDC_H */

// Local Variables:
// c-file-style: "jkent-c"
// End:
