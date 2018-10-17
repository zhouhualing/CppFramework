#include "StdAfx.h"

#include "BLLLoggerFactory.h"
#include "ZipUtils.h"
#include "zip.h"
#include "StcException.h"

#ifdef WIN32
//#define USEWIN32IOAPI
#include "iowin32.h"
#endif


using namespace stc::framework;


const uint32_t ZipUtils::WRITE_BUFFER_SIZE = 16384;
const uint8_t ZipUtils::DEFAULT_COMPRESSION_LEVEL = 6;

#ifdef WIN32
uLong filetime(const char *f, tm_zip *tmzip, uLong *dt)
//char *f;           /* name of file to get info on */
//tm_zip *tmzip;     /* return value: access, modific. and creation times */
//uLong *dt;         /* dostime */
{
    const uint16_t MAX_FILE_NAME_SIZE = 256;

    int ret = 0;
    {
        FILETIME ftLocal;
        HANDLE hFind;
        WIN32_FIND_DATA  ff32;

        hFind = FindFirstFile(f,&ff32);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
            FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
            FindClose(hFind);
            ret = 1;
        }
    }
    return ret;
}
#else
#ifdef unix
uLong filetime(const char *f, tm_zip *tmzip, uLong *dt)
//char *f;           /* name of file to get info on */
//tm_zip *tmzip;     /* return value: access, modific. and creation times */
//uLong *dt;         /* dostime */
{
    int ret=0;
    struct stat s;        /* results of stat() */
    struct tm* filedate;
    time_t tm_t=0;
    const uint16_t MAX_FILE_NAME_SIZE = 256;

    if (strcmp(f,"-")!=0)
    {
        char name[MAX_FILE_NAME_SIZE+1];
        int len = strlen(f);
        if (len > MAX_FILE_NAME_SIZE)
            len = MAX_FILE_NAME_SIZE;

        strncpy(name, f,MAX_FILE_NAME_SIZE-1);
        /* strncpy doesnt append the trailing NULL, of the string is too long. */
        name[ MAX_FILE_NAME_SIZE ] = '\0';

        if (name[len - 1] == '/')
            name[len - 1] = '\0';
        /* not all systems allow stat'ing a file with / appended */
        if (stat(name,&s)==0)
        {
            tm_t = s.st_mtime;
            ret = 1;
        }
    }
    filedate = localtime(&tm_t);

    tmzip->tm_sec  = filedate->tm_sec;
    tmzip->tm_min  = filedate->tm_min;
    tmzip->tm_hour = filedate->tm_hour;
    tmzip->tm_mday = filedate->tm_mday;
    tmzip->tm_mon  = filedate->tm_mon ;
    tmzip->tm_year = filedate->tm_year;

    return ret;
}
#else
uLong filetime(const char *f, tm_zip *tmzip, uLong *dt)
//char *f;           /* name of file to get info on */
//tm_zip *tmzip;     /* return value: access, modific. and creation times */
//uLong *dt;         /* dostime */
{
    return 0;
}
#endif
#endif

bool ZipUtils::ZipFiles(const std::string &archiveFile, const std::vector<std::string> &inputFiles, int compressLevel)
{
    int     err=ZIP_OK;
    int     size_buf=WRITE_BUFFER_SIZE;
    uint8_t buf[WRITE_BUFFER_SIZE];
    const char* filename_try = archiveFile.c_str();
    zipFile zf;
    int     errclose;

    if (compressLevel < 0 || compressLevel > 9)
    {
		std::ostringstream os;
        os << "ZipUtils::ZipFiles Invalid Compression Level: " << compressLevel;
		throw CStcRangeError(os.str());
    }

#ifdef USEWIN32IOAPI
    zlib_filefunc_def ffunc;
    fill_win32_filefunc(&ffunc);
    zf = zipOpen2(filename_try,0,NULL,&ffunc);
#else
    zf = zipOpen(filename_try,0);
#endif

    if (zf == NULL)
    {
		std::ostringstream os;
        os << "ZipUtils::ZipFiles error opening archive: " << filename_try;
		throw CStcRunTimeError(os.str());
    }

    std::ostringstream os;
    for (uint32_t i=0; (i<inputFiles.size()) && (err==ZIP_OK);++i)
    {        
        FILE*   fin;
        int     size_read;
        const char*   filenameinzip = (char *) inputFiles[i].c_str();
        zip_fileinfo zi = {0};

        filetime(filenameinzip,&zi.tmz_date,&zi.dosDate);

        err = zipOpenNewFileInZip(zf,filenameinzip,&zi,
                    NULL,0,NULL,0,NULL /* comment*/,
                    (compressLevel != 0) ? Z_DEFLATED : 0, compressLevel);

        if (err != ZIP_OK)
        {
            os << "ZipUtils::ZipFiles error in opening " << filenameinzip << " in zipfile";
        }
        else
        {
            fin = fopen(filenameinzip,"rb");
            if (fin==NULL)
            {
                err=ZIP_ERRNO;
                os << "ZipUtils::ZipFiles error in opening " << filenameinzip << " for reading";
            }
        }

        if (err == ZIP_OK)
        {
            do
            {
                err = ZIP_OK;
                size_read = (int)fread(buf,1,size_buf,fin);
                if (size_read < size_buf)
                    if (feof(fin)==0)
                    {
                        os << "ZipUtils::ZipFiles error in reading " << filenameinzip;
                        err = ZIP_ERRNO;
                    }

                    if (size_read>0)
                    {
                        err = zipWriteInFileInZip (zf,buf,size_read);
                        if (err<0)
                        {
                            os << "ZipUtils::ZipFiles error in writing " << filenameinzip << " in the zipfile\n";
                        }

                    }
            } while ((err == ZIP_OK) && (size_read>0));
        }
        if (fin)
        {
            fclose(fin);
        }

        if (err<0)
        {
            err=ZIP_ERRNO;
        }
        else
        {
            err = zipCloseFileInZip(zf);
            if (err!=ZIP_OK)
            {
                os <<  "ZipUtils::ZipFiles error in closing " << filenameinzip << " in the zipfile";
            }
        }
    }
    errclose = zipClose(zf,NULL);
    if (errclose != ZIP_OK)
    {
        os << "ZipUtils::ZipFiles error in closing " << filename_try;
        err=ZIP_ERRNO;
    }

    if (err != ZIP_OK)
    {
		throw CStcRunTimeError(os.str());
    }
    return true;
}
