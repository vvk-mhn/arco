/* recplay -- unit generator for arco
 *
 * based on irecplay.cpp from AuraRT
 *
 * Roger B. Dannenberg
 * Apr 2023
 */

#include <cmath>
#include "arcougen.h"
#include "recplay.h"

#define INDEX_TO_BUFFER(i) ((i) / samples_per_buffer)
#define INDEX_TO_OFFSET(i) ((i) % samples_per_buffer)

// the last sample must be obtained from the owner of the samples
#define END_COUNT (lender_ptr ? lender_ptr->sample_count : sample_count)
// similarly, if we're borrowing samples, consult owner for recording status
#define RECORDING (lender_ptr ? lender_ptr->recording : recording)



const char *Recplay_name = "Recplay";

/******
 Computation has 5 cases:
 1. during fade at speed == 1: step through buffer and multiply by
    interpolated fade envelope points.
 2. during fade at speed != 1: interpolate buffer samples (tricky because
    we need previous sample and phase to interpolate, but we also have
    non-contiguous samples in buffers) and use same code as (1) for envelope.
 3. non-fade at speed == 1: simplest case just copies sequential samples
    with test for go-to-next-buffer.
 4. non-fade at speed != 1: no envelope calculation but uses code from (2)
    to interpolate samples from buffers.
 5. not playing: just output zero on all output channels
*******/

// play one channel during fade at speed == 1
//
void Recplay::play_fade_1(int chan, float fade_phase, int buffer, int offset)
{
    Vec<Sample_ptr> &buffers = *states[chan].buffers;
    Sample *src = buffers[buffer] + offset;

    for (int i = 0; i < BL; i++) {
        long iphase = (long) fade_phase; // truncate to integer
        float fraction = fade_phase - iphase; // get fraction 
        // interpolated envelope
        float env = raised_cosine[iphase] * (1 - fraction) +
                    raised_cosine[iphase + 1] * fraction;
        // compute a sample
        *out_samps++ = *src++ * env * *gain_samps;
        fade_phase += fade_incr;
        offset++;
        if (offset >= samples_per_buffer) {
            offset = 0;
            src = (*(states[chan].buffers))[++buffer];
        }
    }
    gain_samps += gain_stride;
}


// play one channel during fade at speed != 1
//    fade_phase - float position (index) within fade curve
//    fade_incr - member variable with per-sample fade_phase increment
//    buffer - index to buffer
//    offset - offset within buffer, corresponds to src, offset > phase
//    phase - float position (index) within buffer for next sample
//    src - pointer to the sample stored at offset in buffers[buffer]
//
void Recplay::play_fade_x(int chan, float fade_phase, int buffer, int offset,
                          double phase)
{
    Vec<Sample_ptr> &buffers = *states[chan].buffers;
    Sample *src = buffers[buffer] + offset;

    Sample next_sample = *src;
    for (int i = 0; i < BL; i++) {
        // note that phase can be negative if offset is zero, i.e. *src is
        // the first sample in the buffer!
        long iphase = (long) fade_phase; // truncate to integer
        float fraction = fade_phase - iphase; // get fraction
        // interpolated envelope
        float env = raised_cosine[iphase] * (1 - fraction) +
                    raised_cosine[iphase + 1] * fraction;
        // compute a sample. frac is the fraction of a sample
        // between the current position (play_phase) and the 
        // location of the next sample (play_index)
        fraction = offset - phase;
        Sample samp = states[chan].prev_sample * (1 - fraction) +
                      next_sample * fraction;
        *out_samps++ = samp * env * *gain_samps;
        fade_phase += fade_incr;
        phase += speed;

        // if speed > 1, we can advance past more than one sample
        if (phase > offset) {
            int n = phase - offset + 1;  // how far to advance, >= 1
            offset += n - 1;  // (re)read prev_sample
            src += n - 1;
            if (offset >= samples_per_buffer) {
                offset = 0;
                phase -= samples_per_buffer;
                src = (*(states[chan].buffers))[++buffer];
            }
            states[chan].prev_sample = *src;
            // now position at the next sample
            offset++;
            src++;
            if (offset >= samples_per_buffer) {
                offset = 0;
                phase -= samples_per_buffer;
                src = (*(states[chan].buffers))[++buffer];
            }
            next_sample = *src;
        }
    }
    gain_samps += gain_stride;
}
    

// play one channel without fade at speed == 1
//    buffer - index to buffer
//    offset - offset within buffer, corresponds to src, offset > phase
//    src - pointer to the sample stored at offset in buffers[buffer]
//
void Recplay::play_1(int chan, int buffer, int offset)
{
    Vec<Sample_ptr> &buffers = *states[chan].buffers;
    Sample *src = buffers[buffer] + offset;

    for (int i = 0; i < BL; i++) {
        // compute a sample
        *out_samps++ = *src++ * *gain_samps;
        offset++;
        if (offset >= samples_per_buffer) {
            offset = 0;
            src = (*(states[chan].buffers))[++buffer];
        }
    }
}


// play one channel without fade at speed != 1
//    buffer - index to buffer
//    offset - offset within buffer, corresponds to src, offset > phase
//    src - pointer to the sample stored at offset in buffers[buffer]
//
void Recplay::play_x(int chan, int buffer, int offset, double phase)
{
    Vec<Sample_ptr> &buffers = *states[chan].buffers;
    Sample *src = buffers[buffer] + offset;

    Sample next_sample = *src;
    for (int i = 0; i < BL; i++) {
        // compute a sample. frac is the fraction of a sample
        // between the current position (play_phase) and the 
        // location of the next sample (play_index)
        float fraction = offset - phase;
        Sample samp = states[chan].prev_sample * (1 - fraction) +
                      next_sample * fraction;
        *out_samps++ = samp * *gain_samps;
        phase += speed;

        // if speed > 1, we can advance past more than one sample
        if (phase > offset) {
            int n = phase - offset + 1;  // how far to advance, >= 1
            offset += n - 1;  // (re)read prev_sample
            src += n - 1;
            if (offset >= samples_per_buffer) {
                offset = 0;
                phase -= samples_per_buffer;
                src = (*(states[chan].buffers))[++buffer];
            }
            states[chan].prev_sample = *src;
            // now position at the next sample
            offset++;
            src++;
            if (offset >= samples_per_buffer) {
                offset = 0;
                phase -= samples_per_buffer;
                src = (*(states[chan].buffers))[++buffer];
            }
            next_sample = *src;
        }
    }
}


void Recplay::real_run()
{
    int buffer = 0;
    long offset = 0;
    double phase = 0;

    input_samps = input->run(current_block);
    gain_samps = gain->run(current_block);
    if (lender_ptr) {
        lender_ptr->run(current_block);
    }

    if (recording) {
        int buffer = (int) INDEX_TO_BUFFER(rec_index);
        int offset = (int) INDEX_TO_OFFSET(rec_index);

        // expand all channel storage if we need more space to record
        if (buffer >= states[0].my_buffers.size()) {
            assert(buffer == states[0].my_buffers.size());
            for (int i = 0; i < chans; i++) {
                Sample_ptr b = O2_MALLOCNT(SAMPLES_PER_BUFFER, Sample);
                assert(b);
                states[i].my_buffers.push_back(b);
                if (flags & UGENTRACE) {
                    int n = states[i].my_buffers.size();
                    if (n % 10 == 0) {
                        arco_print("Recorded %g sec in ",
                                   AP * n * SAMPLES_PER_BUFFER);
                        print();
                    }
                }
            }
        }
        
        // record the input
        for (int i = 0; i < chans; i++) {
            block_copy(states[i].my_buffers[buffer] + offset, input_samps);
            input_samps += input_stride;
        }
        rec_index += BL;
        if (rec_index > sample_count) sample_count = rec_index;
        
    }

    if (playing) {
        // fade out if we are near the end. This code works on block
        // boundaries so it may fade up to one block early (<1ms)
        // the test for !stopping is redundant but helps with debugging by
        // not calling stop multiple times. If we are recording, the 
        // END_COUNT is increasing, so we only worry about running out of
        // samples when speed > 1.
        //     "+ 1 + BL" is correct: round up fade_len * speed,
        // and then we need extra for interpolation.
        // note that fade_len is derived from fade when playback is
        // started. You cannot change the fade-out time after you start.
        if (!stopping) {
            if ((!RECORDING || speed >= 1) &&
                (play_index + long(fade_len * speed) + 1 + BL >= END_COUNT)) {
                stop();
            }
        }
        // If the record buffer is very short, or if speed has changed,
        // it's possible that there is not enough time to fade out, 
        // in which case we could overrun the buffers and try to 
        // access non-existent memory. Test for overrun now, and just
        // stop processing if we're out of samples.
        // 
        // To continue, we need BL samples, so the final sample phase
        // will be:
        double final_phase = play_phase + (BL - 1) * speed;        
        // With interpolation, we need one sample beyond final_phase
        // (and to keep this simple, we'll always arrange for fades to
        // end one sample early so that if we round up, we'll still be
        // in bounds.)
        long last_offset = long(final_phase + 1);
        // Now we can see if we're going to run out of samples:
        if (last_offset >= END_COUNT) {
            // Note that in some cases we might run out of samples in the
            // middle of a buffer, so we could actually output a few samples.
            // Thus, we might drop a fraction of a block of recorded samples.
            // The extra code to handle this does not seem worth the effort.
            // If the recording time is reasonable and if the speed is not
            // changing, this code will never be reached because the fadeout
            // will finish before we run out of samples.
            block_zero_n(out_samps, chans);
            playing = false;
            arco_warn("Recplay sudden stop, out of samples"); print();
            if (flags & CAN_TERMINATE) {
                terminate(ACTION_ERROR | ACTION_END);
            } else {
                send_action_id(ACTION_ERROR | ACTION_END);
            }
            return;
        }
        int buffer = (int) INDEX_TO_BUFFER(play_index);
        int offset = INDEX_TO_OFFSET(play_index);
        // phase is exact offset within buffer. Computed by subtracting
        // off the length of the previous buffers, which is play_index
        // minus offset, which is where we are in the current buffer.
        // If offset == 0, phase can be < 0.
        double phase = play_phase - (play_index - offset);

        if (fading) {
            if (speed == 1.0) {
                for (int i = 0; i < chans; i++) {
                    play_fade_1(i, fade_phase, buffer, offset);
                }
                play_index += BL;
                play_phase = play_index;
            } else {
                for (int i = 0; i < chans; i++) {
                    play_fade_x(i, fade_phase, buffer, offset, phase);
                }
                play_phase += BL * speed;
                play_index = std::ceil(play_phase);  // round up
            }
            fade_phase += fade_incr * BL;
            fade_count--;
            if (fade_count <= 0) {
                fading = false;
                if (stopping) { // stopping means fading out, not in
                    stopping = false;
                    playing = false;
                    if (loop) {
                        start(start_time);
                    } else {
                        if (flags & CAN_TERMINATE) {
                            terminate(ACTION_END);
                        } else {
                            send_action_id(ACTION_END);
                        }
                    }
                }
            } // else we finished fading in
        } else {  // not fading
            if (speed == 1.0) {
                for (int i = 0; i < chans; i++) {
                    play_1(i, buffer, offset);
                }
                play_index += BL;
                play_phase = play_index;
            } else {
                for (int i = 0; i < chans; i++) {
                    play_x(i, buffer, offset, phase);
                }
                play_phase += BL * speed;
                play_index = std::ceil(play_phase);  // round up
            }                
        }
    } else { // !playing, so just output zeros and check for termination
        block_zero_n(out_samps, chans);
        if (flags & CAN_TERMINATE) {
            terminate(ACTION_END);
        }  // else we already did send_action_id
    }
}


void Recplay::record(bool record) {
    if (lender_ptr) {
        arco_warn("Recplay: can't record into lender's buffer\n");
        return;
    }
    if (!states[0].buffers) {
        for (int i = 0; i < chans; i++) {
            states[i].my_buffers.init(10);  // allocate space for some buffer pointers
            states[i].buffers = &states[i].my_buffers;
        }
    }
    if (recording == record) {
        return;  // already recording/not recording
    }
    if (record) {
        rec_index = 0;
        recording = true;
        sample_count = 0;
    } else {
        recording = false;
        printf("Recplay::record stop, rec_index %ld end_count %ld\n",
               rec_index, sample_count);
    }
}


void Recplay::start(double start_time_)
{
    start_time = start_time_;
    if (!states[0].buffers) {
        arco_warn("Recplay: Nothing recorded to play");
        goto no_play;
    }
    play_index = (long) (start_time * AR);
    if (play_index < 0) {
        play_index = 0;
    } else if (play_index > END_COUNT) {
        play_index = END_COUNT;
    }
    play_phase = play_index;
    for (int i = 0; i < chans; i++) {  // just to be safe, initialize
        states[i].prev_sample = 0;     // for interpolation
    }
    fade_len = long(fade * AR);
    fade_count = fade_len * BL_RECIP;
    if (fade_len <= 0) {
        arco_warn("Recplay: fade_len too short, not starting.");
        goto no_play;
    }
    if (!RECORDING) {
        // round up to integral number of blocks
        long needed_blocks = long(fade_count * speed) + 1;
        if (needed_blocks <= 0) needed_blocks = 1;
        long recorded_blocks = (END_COUNT - play_index) / BL;
        if (needed_blocks * 2 > recorded_blocks) {
            // If fade is long and recording is short, there's no time for
            // fade-in/fade-out, and the anticipatory fade-out will set the
            // gain to 1 and cause a glitch. Therefore, split the recording
            // time to allow a fade-in followed by a fade-out.
            fade_count = long(recorded_blocks / (2 * speed));
            fade_len = fade_count * BL;
            if (fade_len <= 0) {
                arco_warn("Recplay: can't play very short recording");
                goto no_play;
            } else {
                arco_warn(
                        "Recplay: fade-in time cut to half of recording len.");
            }
        }
    }

    fade_incr = 100.0 / fade_len;
    fade_phase = 2.0; // phase goes from 2 to 102

    fading = true;
    stopping = false;
    playing = true;
    return;
no_play:
    send_action_id(ACTION_EXCEPT);
}


void Recplay::stop()
{
    // note: fading in and out are done on block boundaries to
    // simplify processing
    if (playing && !stopping) {
        fading = true;
        stopping = true;
        fade_count = long(fade * AR * BL_RECIP);
        fade_len = fade_count * BL;

        if (!RECORDING) {
            long needed_blocks = long(fade_count * speed + 1);
            // in case speed is zero or negative, force something reasonable
            if (needed_blocks <= 0) needed_blocks = 1;
            // do we have time to fade out?
            long blocks_left = ((END_COUNT - play_index) / BL) - 1;
            if (needed_blocks > blocks_left) {
                fade_count = long(blocks_left / speed);
                fade_len = fade_count * BL;
            }
        }
        // in case fade is zero or negative, force something reasonable
        if (fade_count <= 0) {
            fade_count = 1;
            fade_len = BL;
        }
        // on the other hand, if we're recording, we have more
        // time to fade, but since recording may stop, and we may
        // have speed > 1, we can still run out of samples to play.
        // This may cause a glitch and a sudden stop.

        fade_incr = -(fade_phase - 2.0) / fade_len;
        // phase will go from current value (normally 102.0) to 2.0
    }
}


void Recplay::borrow(int lender_id)
{
    if (states[0].buffers) {
        arco_warn("Irecplay::borrow called after buffer allocation");
        return;
    }

    UGEN_FROM_ID(Recplay, lender, lender_id, "arco_recplay_borrow lender");

    if (borrowing) { // return buffers to lender
        lender_ptr->unref();
        borrowing = false;
        for (int i = 0; i < chans; i++) {
            states[i].buffers = NULL;
        }
    }

    if (!lender) return;
    // make sure we are borrowing buffers from a lender of class Recplay
    if (lender->classname() != Recplay_name) {
        arco_error("Recplay::borrow - lender %d has class %s\n",
                   lender_id, lender->classname());
        return;
    }
    
    // map buffers from lender to borrower round-robin
    int j = 0;
    for (int i = 0; i < chans; i++) {
        states[i].buffers = &(lender->states[j].my_buffers);
        j = (j + 1) % lender->chans;
    }
    lender->ref();
    lender_ptr = lender;
    borrowing = true;
}


/* O2SM INTERFACE: /arco/recplay/new int32 id, int32 chans, 
              int32 input_id, int32 gain_id, float fade, bool loop;
 */
void arco_recplay_new(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t chans = argv[1]->i;
    int32_t input_id = argv[2]->i;
    int32_t gain_id = argv[3]->i;
    float fade = argv[4]->f;
    bool loop = argv[5]->B;
    // end unpack message

    ANY_UGEN_FROM_ID(input, input_id, "arco_recplay_new inp");
    ANY_UGEN_FROM_ID(gain, gain_id, "arco_recplay_new gain");
    new Recplay(id, chans, input, gain, fade, loop);
}


/* O2SM INTERFACE: /arco/recplay/repl_input int32 id, int32 input_id;
 */
static void arco_recplay_repl_input(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t input_id = argv[1]->i;
    // end unpack message

    UGEN_FROM_ID(Recplay, recplay, id, "arco_recplay_repl_input");
    ANY_UGEN_FROM_ID(input, input_id, "arco_recplay_repl_input");
    recplay->repl_input(input);
}


/* O2SM INTERFACE: /arco/recplay/repl_gain int32 id, int32 gain_id;
 */
static void arco_recplay_repl_gain(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t gain_id = argv[1]->i;
    // end unpack message

    UGEN_FROM_ID(Recplay, recplay, id, "arco_recplay_repl_gain");
    ANY_UGEN_FROM_ID(gain, gain_id, "arco_recplay_repl_gain");
    recplay->repl_gain(gain);
}


/* O2SM INTERFACE: /arco/recplay/set_gain int32 id, int32 chan, float gain;
 */
static void arco_recplay_set_gain(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t chan = argv[1]->i;
    float gain = argv[2]->f;
    // end unpack message

    UGEN_FROM_ID(Recplay, recplay, id, "arco_recplay_repl_gain");
    recplay->set_gain(chan, gain);
}


/* O2SM INTERFACE: /arco/recplay/speed int32 id, float speed;
 */
static void arco_recplay_speed(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    float speed = argv[1]->f;
    // end unpack message

    UGEN_FROM_ID(Recplay, recplay, id, "arco_recplay_speed");
    recplay->set_speed(speed);
}


/* O2SM INTERFACE: /arco/recplay/rec int32 id, bool record;
 */
static void arco_recplay_rec(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    bool record = argv[1]->B;
    // end unpack message

    UGEN_FROM_ID(Recplay, recplay, id, "arco_recplay_rec");
    recplay->record(record);
}


/* O2SM INTERFACE: /arco/recplay/start int32 id, float start_time;
 */
static void arco_recplay_start(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    float start_time = argv[1]->f;
    // end unpack message

    UGEN_FROM_ID(Recplay, recplay, id, "arco_recplay_start");
    recplay->start(start_time);
}


/* O2SM INTERFACE: /arco/recplay/stop int32 id;
 */
static void arco_replay_stop(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    // end unpack message

    UGEN_FROM_ID(Recplay, recplay, id, "arco_recplay_stop");
    recplay->stop();
}


/* O2SM INTERFACE: /arco/recplay/borrow int32 id, int32 lender_id;
 */
static void arco_replay_borrow(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t lender_id = argv[1]->i;
    // end unpack message

    UGEN_FROM_ID(Recplay, recplay, id, "arco_recplay_borrow id");
    recplay->borrow(lender_id);
}


static void recplay_init()
{
    // O2SM INTERFACE INITIALIZATION: (machine generated)
    o2sm_method_new("/arco/recplay/new", "iiiifB", arco_recplay_new, NULL,
                    true, true);
    o2sm_method_new("/arco/recplay/repl_inp", "ii", arco_recplay_repl_input,
                    NULL, true, true);
    o2sm_method_new("/arco/recplay/repl_gain", "ii", arco_recplay_repl_gain,
                    NULL, true, true);
    o2sm_method_new("/arco/recplay/set_gain", "iif", arco_recplay_set_gain,
                    NULL, true, true);
    o2sm_method_new("/arco/recplay/speed", "if", arco_recplay_speed, NULL,
                    true, true);
    o2sm_method_new("/arco/recplay/rec", "iB", arco_recplay_rec, NULL, true,
                    true);
    o2sm_method_new("/arco/recplay/start", "if", arco_recplay_start, NULL,
                    true, true);
    o2sm_method_new("/arco/recplay/stop", "i", arco_replay_stop, NULL, true,
                    true);
    o2sm_method_new("/arco/recplay/borrow", "ii", arco_replay_borrow, NULL,
                    true, true);
    // END INTERFACE INITIALIZATION
}

Initializer recplay_init_obj(recplay_init);
