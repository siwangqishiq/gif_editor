package panyi.xyz.textrender.data

import java.io.Serializable

open class PickerFile : Serializable {
    var name: String? = null
    var path: String? = null
    var duration: Long = 0
    var width: Int = 0
    var height: Int = 0
    var size: Long = 0
    var bitrate: String? = null
    var album: String? = null
}