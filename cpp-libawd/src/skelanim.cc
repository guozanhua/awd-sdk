#include "util.h"
#include "skelanim.h"

#include "platform.h"


AWDSkeletonPose::AWDSkeletonPose(const char *name, awd_uint16 name_len) :
    AWDNamedElement(name, name_len), AWDAttrElement(), AWDBlock(SKELETON_POSE)
{
    this->num_transforms = 0;
    this->first_transform = NULL;
    this->last_transform = NULL;
}


AWDSkeletonPose::~AWDSkeletonPose()
{
    AWD_joint_tf *cur;

    cur = this->first_transform;
    while (cur) {
        AWD_joint_tf *next = cur->next;
        cur->next = NULL;
        if (cur->transform_mtx) {
            free(cur->transform_mtx);
            cur->transform_mtx = NULL;
        }
        free(cur);
        cur = next;
    }

    this->num_transforms = 0;
    this->first_transform = NULL;
    this->last_transform = NULL;
}


void
AWDSkeletonPose::set_next_transform(awd_float64 *mtx)
{
    AWD_joint_tf *tf;

    tf = (AWD_joint_tf*)malloc(sizeof(AWD_joint_tf));
    tf->transform_mtx = mtx;

    if (this->first_transform == NULL) {
        this->first_transform = tf;
    }
    else {
        this->last_transform->next = tf;
    }

    this->num_transforms++;
    this->last_transform = tf;
    this->last_transform->next = NULL;
}


awd_uint32
AWDSkeletonPose::calc_body_length(BlockSettings * curBlockSettings)
{
    awd_uint32 len;
    AWD_joint_tf *cur;

    len = this->get_name_length() + 4; // strlen field + num transforms
    len += this->calc_attr_length(true,true, curBlockSettings);

    cur = this->first_transform;
    while (cur) {
        len += sizeof(awd_bool);
        if (cur->transform_mtx != NULL)
            len += MTX43_SIZE(curBlockSettings->get_wide_matrix());

        cur = cur->next;
    }

    return len;
}


void
AWDSkeletonPose::write_body(int fd, BlockSettings * curBlockSettings)
{
    awd_bool bt;
    awd_bool bf;
    AWD_joint_tf *cur;
    awd_uint16 num_joints_be;

    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    num_joints_be = UI16(this->num_transforms);
    write(fd, &num_joints_be, sizeof(awd_uint16));

    this->properties->write_attributes(fd,  curBlockSettings);

    bt = AWD_TRUE;
    bf = AWD_FALSE;
    cur = this->first_transform;
    while (cur) {
        if (cur->transform_mtx) {
            write(fd, &bt, 1);
			awd_float32 offX=awd_float32(cur->transform_mtx[9]*curBlockSettings->get_scale());
			awd_float32 offY=awd_float32(cur->transform_mtx[10]*curBlockSettings->get_scale());
			awd_float32 offZ=awd_float32(cur->transform_mtx[11]*curBlockSettings->get_scale());
			awdutil_write_floats(fd, cur->transform_mtx, 9, curBlockSettings->get_wide_matrix());
			if (!curBlockSettings->get_wide_matrix()){
				write(fd, &offX, sizeof(awd_float32));
				write(fd, &offY, sizeof(awd_float32));
				write(fd, &offZ, sizeof(awd_float32));
			}
			else{
				write(fd, &offX, sizeof(awd_float64));
				write(fd, &offY, sizeof(awd_float64));
				write(fd, &offZ, sizeof(awd_float64));
			}
        }
        else {
            write(fd, &bf, 1);
        }

        cur = cur->next;
    }

    this->user_attributes->write_attributes(fd,  curBlockSettings);
}





AWDSkeletonAnimation::AWDSkeletonAnimation(const char *name, awd_uint16 name_len, int start_frame, int end_frame, int skip_frame, bool stitch_final, const char * sourceID) :
    AWDNamedElement(name, name_len), AWDAttrElement(), AWDBlock(SKELETON_ANIM)
{
    this->num_frames = 0;
    this->first_frame = NULL;
    this->last_frame = NULL;
	this->start_frame = start_frame;
	this->end_frame = end_frame;
	this->skip_frame = skip_frame;
	this->stitch_final = stitch_final;
	this->sourceID_len = 0;
	this->set_sourceID(sourceID, strlen(sourceID));
	this->is_processed=false;
}


AWDSkeletonAnimation::~AWDSkeletonAnimation()
{
    AWD_skelanim_fr *cur;

    cur = this->first_frame;
    while (cur) {
        AWD_skelanim_fr *next = cur->next;
        cur->next = NULL;
        delete cur->pose;
        free(cur);
        cur = next;
    }
	if(this->sourceID_len>0){
		free(this->sourceID);
		this->sourceID_len=0;
	}

    this->first_frame = NULL;
    this->last_frame = NULL;
}

char *
AWDSkeletonAnimation::get_sourceID()
{
    return this->sourceID;
}

void
AWDSkeletonAnimation::set_sourceID(const char *name, awd_uint16 name_len)
{
	if(this->sourceID_len>0)
		free(this->sourceID);
    this->sourceID_len = name_len;
    if (name != NULL) {
        this->sourceID = (char*)malloc(this->sourceID_len+1);
        strncpy(this->sourceID, name, this->sourceID_len);
        this->sourceID[this->sourceID_len] = 0;
    }
}

bool
AWDSkeletonAnimation::get_is_processed()
{
	return this->is_processed;
}

void
AWDSkeletonAnimation::set_is_processed(bool isProcessed)
{
    this->is_processed = isProcessed;
}
int
AWDSkeletonAnimation::get_start_frame()
{
	return this->start_frame;
}
int
AWDSkeletonAnimation::get_end_frame()
{
	return this->end_frame;
}
int
AWDSkeletonAnimation::get_skip_frame()
{
	return this->skip_frame;
}
bool
AWDSkeletonAnimation::get_stitch_final()
{
	return this->stitch_final;
}

void
AWDSkeletonAnimation::set_next_frame_pose(AWDSkeletonPose *pose, awd_uint16 duration)
{
    AWD_skelanim_fr *fr;

    fr = (AWD_skelanim_fr *)malloc(sizeof(AWD_skelanim_fr));
    fr->pose = pose;
    fr->duration = duration;

    if (this->first_frame == NULL) {
        this->first_frame = fr;
    }
    else {
        this->last_frame->next = fr;
    }

    this->num_frames++;
    this->last_frame = fr;
    this->last_frame->next = NULL;
}


void
AWDSkeletonAnimation::prepare_and_add_dependencies(AWDBlockList *export_list)
{
	
    AWD_skelanim_fr *frame;
    frame = this->first_frame;
    while (frame) {
		frame->pose->prepare_and_add_with_dependencies(export_list);
        frame = frame->next;
    }
}

awd_uint32
AWDSkeletonAnimation::calc_body_length(BlockSettings * curBlockSettings)
{
    awd_uint32 len;
    awd_uint8 pose_len;

    pose_len = sizeof(awd_baddr)+sizeof(awd_uint16);

    len = sizeof(awd_uint16) + this->get_name_length();             // Name varstr
    len += sizeof(awd_uint16);                                      // num frames
    len += (this->num_frames * pose_len);                           // Pose list
    len += this->calc_attr_length(true,true, curBlockSettings);             // Props and attributes
    
    return len;
}


void
AWDSkeletonAnimation::write_body(int fd, BlockSettings * curBlockSettings)
{
    AWD_skelanim_fr *frame;
    awd_uint16 num_frames_be;

    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());

    num_frames_be = UI16(this->num_frames);
    write(fd, &num_frames_be, sizeof(awd_uint16));

	this->properties->write_attributes(fd, curBlockSettings);

    frame = this->first_frame;
    while (frame) {
        awd_baddr addr_be = UI32(frame->pose->get_addr());
        awd_uint16 dur_be = UI16(frame->duration);

        write(fd, &addr_be, sizeof(awd_baddr));
        write(fd, &dur_be, sizeof(awd_uint16));

        frame = frame->next;
    }

    this->user_attributes->write_attributes(fd,  curBlockSettings);
}
