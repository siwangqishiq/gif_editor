package panyi.xyz.gifeditor.activity

import android.app.Activity
import android.content.ContentUris
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.provider.MediaStore
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.ImageView
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.database.getStringOrNull
import androidx.recyclerview.widget.GridLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.bumptech.glide.Glide
import panyi.xyz.gifeditor.R
import panyi.xyz.gifeditor.data.FilePickerParams
import xyz.panyi.textrender.util.Log

/**
 * 媒体文件选择
 */
class FilePickerActivity : AppCompatActivity() {
    companion object{
        const val TAG = "FilePickerActivity"
        
        const val INTENT_PARAMS = "_params"
        const val RESULT_SELECTED_FILES = "_selected_files"
        
        fun start(ctx: Activity, type:Int, requestCode : Int){
            ctx.startActivityForResult( Intent(ctx, FilePickerActivity::class.java).apply {
                putExtra(INTENT_PARAMS, FilePickerParams(type))
            },requestCode)
        }
    }
    
    private var params: FilePickerParams? = null
    private lateinit var listView:RecyclerView
    private lateinit var confirmButton: Button
    private val videoList = mutableListOf<VideoItem>()
    private val selectedItems = mutableListOf<String>()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_file_picker)
        initView()
        readParams()
        loadVideos()
    }
    
    private fun initView(){
        setSupportActionBar(findViewById(R.id.toolbar))
        setTitle(R.string.picker_title)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)
        supportActionBar?.setDisplayShowHomeEnabled(true)
        
        listView = findViewById(R.id.picker_file_list)
        listView.layoutManager =GridLayoutManager(this,3)
        listView.adapter = VideoAdapter()

        confirmButton = findViewById(R.id.confirm_button)
        confirmButton.setOnClickListener {
            finish() // 调用finish()会返回选择的视频
        }
        updateConfirmButtonState() // 初始化按钮状态
    }

    // 更新确认按钮状态
    private fun updateConfirmButtonState() {
        confirmButton.isEnabled = selectedItems.isNotEmpty()
    }

    private fun readParams(){
        params = intent.getSerializableExtra(INTENT_PARAMS) as FilePickerParams
        Log.i(TAG, "pick file type : ${params?.type}")
    }

    private fun loadVideos(){
        Thread{
            val projection = arrayOf(
                MediaStore.Video.Media._ID,
                MediaStore.Video.Media.DISPLAY_NAME,
                MediaStore.Video.Media.DURATION,
                MediaStore.Video.Media.SIZE,
                MediaStore.Video.Media.DATE_ADDED,
                MediaStore.Video.Media.DATA
            )
            val sortOrder = "${MediaStore.Video.Media.DATE_ADDED} DESC"
            contentResolver.query(MediaStore.Video.Media.EXTERNAL_CONTENT_URI,projection,
                "${MediaStore.Video.Media.DURATION } > ?",
                arrayOf<String>("1000"),
                sortOrder)?.use { cursor ->
                    val idColumn = cursor.getColumnIndexOrThrow(MediaStore.Video.Media._ID)
                    val nameColumns = cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DISPLAY_NAME)
                    val durationColumn = cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DURATION)
                    val sizeColumn = cursor.getColumnIndexOrThrow(MediaStore.Video.Media.SIZE)
                    val pathColumn = cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DATA)
                    while (cursor.moveToNext()) {
                         val id =  cursor.getLong(idColumn)
                         val name = cursor.getString(nameColumns)
                         val duration = cursor.getLong(durationColumn)
                         val size = cursor.getLong(sizeColumn)
                        val path = cursor.getStringOrNull(pathColumn)
                         val contentUri = ContentUris.withAppendedId(
                             MediaStore.Video.Media.EXTERNAL_CONTENT_URI,
                             id
                         )
                         videoList.add(VideoItem(contentUri, name, duration, size, path?:""))
                    }
            }
            runOnUiThread{
                listView.adapter?.notifyItemRangeChanged(0,videoList.size)
            }
        }.start()
    }

    override fun onSupportNavigateUp(): Boolean {
        finish()
        return true
    }

    override fun finish() {
        if (selectedItems.isNotEmpty()) {
            val result = Intent().apply {
                putExtra(RESULT_SELECTED_FILES, selectedItems.toTypedArray())
            }
            setResult(RESULT_OK, result)
        } else {
            setResult(RESULT_CANCELED)
        }
        super.finish()
    }

    // 视频项数据类
    private data class VideoItem(
        val uri: Uri,
        val name: String,
        val duration: Long,
        val size: Long,
        val path:String
    )

    // 视频列表适配器
    private inner class VideoAdapter : RecyclerView.Adapter<VideoAdapter.VideoViewHolder>() {

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): VideoViewHolder {
            val view = LayoutInflater.from(parent.context)
                .inflate(R.layout.item_video, parent, false)
            return VideoViewHolder(view)
        }

        override fun onBindViewHolder(holder: VideoViewHolder, position: Int) {
            val item = videoList[position]
            holder.bind(item)
        }

        override fun getItemCount(): Int = videoList.size

        inner class VideoViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
            private val thumbnail: ImageView = itemView.findViewById(R.id.video_thumbnail)
            private val durationText: TextView = itemView.findViewById(R.id.video_duration)
            private val selectedOverlay: View = itemView.findViewById(R.id.selected_overlay)

            fun bind(item: VideoItem) {
                // 加载缩略图
                Glide.with(itemView.context)
                    .load(item.uri)
                    .thumbnail(0.1f)
                    .centerCrop()
                    .into(thumbnail)

                // 格式化显示时长 (mm:ss)
                val minutes = (item.duration / 1000) / 60
                val seconds = (item.duration / 1000) % 60
                durationText.text = String.format("%02d:%02d", minutes, seconds)

                // 选中状态
                val isSelected = selectedItems.contains(item.path)
                selectedOverlay.visibility = if (isSelected) View.VISIBLE else View.GONE

                // 点击事件
                itemView.setOnClickListener {
                    if (params?.maxSelectCount == 1) {
                        selectedItems.clear()
                        selectedItems.add(item.path)
                        finish()
                    } else {
                        if (selectedItems.contains(item.path)) {
                            selectedItems.remove(item.path)
                        } else {
                            if (selectedItems.size < (params?.maxSelectCount ?: 1)) {
                                selectedItems.add(item.path)
                            }
                        }
                        notifyItemChanged(adapterPosition)
                        updateConfirmButtonState() // 更新按钮状态
                    }
                }
            }
        }
    }
}//end class