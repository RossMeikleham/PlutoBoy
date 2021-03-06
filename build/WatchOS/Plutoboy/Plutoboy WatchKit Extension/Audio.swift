import Foundation
import AVFoundation
import Accelerate


class AudioManager {
    
    let OUT_SAMPLE_RATE : Double = 44100;
    let CHANNEL_COUNT : UInt32 = 2;
    let SOUND_QUEUE_SAMPLE_COUNT : UInt32 = 8192;
    
    var SOUND_QUEUE_IN_BUFFER_SIZE : UInt32;
    var SOUND_QUEUE_OUT_BUFFER_SIZE : UInt32;
    
    var rawAudioInData : UnsafeMutablePointer<Int16>;
    var rawAudioOutDataChannel0 : [Float];
    var rawAudioOutDataChannel1 : [Float];
    
    var outAudioFormat: AVAudioFormat;
    var outAudioBuffer: AVAudioPCMBuffer;
    var outChannel0: UnsafeMutablePointer<Float32>;
    var outChannel1: UnsafeMutablePointer<Float32>;

    var audioEngine: AVAudioEngine!;
    var audioPlayer: AVAudioPlayerNode!;
    var session: AVAudioSession;
    
    init() {
  
        session = AVAudioSession.sharedInstance()
        do {try session.setCategory(AVAudioSession.Category.playback,
                            mode: .default,
                            policy: .longForm,
                            options: [])
        }
        catch {
            print(error);
        }
        
        SOUND_QUEUE_IN_BUFFER_SIZE = SOUND_QUEUE_SAMPLE_COUNT * UInt32(MemoryLayout<Int16>.size);
        SOUND_QUEUE_OUT_BUFFER_SIZE = SOUND_QUEUE_SAMPLE_COUNT * UInt32(MemoryLayout<Float32>.size);
    
        self.audioEngine = AVAudioEngine();
        self.audioPlayer = AVAudioPlayerNode();
        
        
        self.rawAudioInData = UnsafeMutablePointer<Int16>.allocate(capacity:Int(SOUND_QUEUE_SAMPLE_COUNT * 2));
        for i in 0..<SOUND_QUEUE_SAMPLE_COUNT * 2 {
            (rawAudioInData + UnsafeMutablePointer<Int16>.Stride(i)).initialize(to: 0x7FFF)
        }
        
        
        self.outAudioFormat = AVAudioFormat(commonFormat: .pcmFormatFloat32, sampleRate: OUT_SAMPLE_RATE, channels: CHANNEL_COUNT, interleaved: false)!;
        
        let numFrames = UInt32(SOUND_QUEUE_SAMPLE_COUNT);
        
        self.outAudioBuffer = AVAudioPCMBuffer(pcmFormat: outAudioFormat, frameCapacity: numFrames / 2)!;
        self.outAudioBuffer.frameLength = self.outAudioBuffer.frameCapacity;
        
        let channels = UnsafeBufferPointer(start: outAudioBuffer.floatChannelData, count: Int(outAudioBuffer.format.channelCount));
        
        self.outChannel0 = channels[0];
        self.outChannel1 = channels[1];
        
        //self.speedControl = AVAudioUnitVarispeed();
        //self.speedControl.rate = 2.0;
        
        let mainMixer = self.audioEngine.mainMixerNode;
        
        self.audioEngine.attach(audioPlayer);
        //self.audioEngine.attach(speedControl);
        //self.audioEngine.connect(audioPlayer, to: speedControl, format: nil)
        //self.audioEngine.connect(speedControl, to:mainMixer, format: mainMixer.outputFormat(forBus: 0));
        self.audioEngine.connect(audioPlayer, to:mainMixer, format: mainMixer.outputFormat(forBus: 0));
        
        self.rawAudioOutDataChannel0 = [Float](repeating: 0.0, count: Int(SOUND_QUEUE_SAMPLE_COUNT) / 2);
        self.rawAudioOutDataChannel1 = [Float](repeating: 0.0, count: Int(SOUND_QUEUE_SAMPLE_COUNT) / 2);
    }
    
    public func activate(do_after:() ->()) {
        self.session.activate(options: []) { (success, error) in
            do {
                try self.audioEngine.start()
                self.audioPlayer.play()
                self.playSounds();
            
            // If something went wrong with enabling sound, or
            // the user just didn't want to use it, then disable
            // it and continue
            } catch {
                print("Didnt enable sound");
                disable_sound_queue();
            };
        }
        do_after();
    }
    
    public func playSounds() {
      
        sound_queue_read(rawAudioInData, SOUND_QUEUE_IN_BUFFER_SIZE);
        
        // Int16 ranges from -32768 to 32767 -- we want to convert and scale these to Float values between -1.0 and 1.0
        var scale = Float(Int16.max) + 1.0
    
        // Samples read alternate Left/Right Channel Data, need to split these up,
        //convert to Float32, and send to each individual channel:
        
        vDSP_vflt16(UnsafePointer(rawAudioInData), 2, &rawAudioOutDataChannel0, 1, vDSP_Length(SOUND_QUEUE_SAMPLE_COUNT / 2));
        vDSP_vflt16(UnsafePointer(rawAudioInData) + UnsafeMutablePointer<Int16>.Stride(1), 2 , &rawAudioOutDataChannel1, 1, vDSP_Length(SOUND_QUEUE_SAMPLE_COUNT / 2));
        
        // Divide by scale
        vDSP_vsdiv(rawAudioOutDataChannel0, 1, &scale, &rawAudioOutDataChannel0, 1, vDSP_Length(SOUND_QUEUE_SAMPLE_COUNT/2))
        vDSP_vsdiv(rawAudioOutDataChannel1, 1, &scale, &rawAudioOutDataChannel1, 1, vDSP_Length(SOUND_QUEUE_SAMPLE_COUNT/2))
        
        memcpy(self.outChannel0, rawAudioOutDataChannel0, Int(SOUND_QUEUE_OUT_BUFFER_SIZE) / 2);
        memcpy(self.outChannel1, rawAudioOutDataChannel1, Int(SOUND_QUEUE_OUT_BUFFER_SIZE) / 2);
        
        // Do the playing
        self.audioPlayer.scheduleBuffer(outAudioBuffer, completionHandler: {
            self.playSounds();
        })
  }
}
