package panyi.xyz.textrender.data

import java.io.Serializable

const val PICKER_TYPE_IMAGE = 1 //图片
const val PICKER_TYPE_VIDEO = 2 //视频

/**
 *  选择媒体文件参数
 */
data class FilePickerParams (
    val type:Int,
    val multiSelected : Boolean = false,
    var minSelectCount:Int = 1, //最小选择数量
    var maxSelectCount:Int = 3, //最大选择数量
): Serializable
