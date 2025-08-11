package panyi.xyz.gifeditor.util

import android.content.Context
import android.net.Uri
import android.os.Build
import android.provider.MediaStore
import java.io.File
import java.io.FileOutputStream


object FileUtil {
    fun convertUriToPath(context: Context, uri: Uri) : String?{
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
            return getRealPathFromURI(context, uri) // 老版本可直接取
        } else {
            val file = copyUriToCache(context, uri) // 高版本复制到缓存
            return file.absolutePath
        }
    }
    
    fun getRealPathFromURI(context: Context, uri: Uri): String? {
        val projection = arrayOf<String?>(MediaStore.Video.Media.DATA)
        context.contentResolver.query(uri, projection, null,
            null, null).use { cursor ->
            if (cursor != null && cursor.moveToFirst()) {
                val columnIndex = cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DATA)
                return cursor.getString(columnIndex)
            }
        }
        return null
    }
    
    
    fun copyUriToCache(context: Context, uri: Uri): File {
        val cacheFile = File(context.cacheDir,
            "temp_video_${System.currentTimeMillis()}.mp4")
        context.contentResolver.openInputStream(uri).use { inputStream ->
            FileOutputStream(cacheFile).use { out ->
                val buffer = ByteArray(1024 * 1024)
                var len: Int
                while ((inputStream!!.read(buffer).also { len = it }) != -1) {
                    out.write(buffer, 0, len)
                }
            }
        }
        return cacheFile
    }
}