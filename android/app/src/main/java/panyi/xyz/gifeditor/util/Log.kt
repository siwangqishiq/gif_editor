package xyz.panyi.textrender.util

import android.content.Context
import android.util.Log

/**
 * 日志显示
 * 三种日志级别  info / warn / error
 *
 */
object Log {
    private const val TAG:String = "Logger"
    
    fun init(ctx: Context){
        log("Log init")
    }
    
    fun i(tag:String, msg:String?){
        Log.i(tag, msg?:"")
    }
    
    fun e(tag:String, msg:String?){
        Log.e(tag,msg?:"")
    }
    
    fun w(tag:String, msg:String?){
        Log.w(tag, msg?:"")
    }
    
    fun log(msg:String?){
        i(TAG, msg)
    }
}//end class