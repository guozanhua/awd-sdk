#include "material.h"
#include "util.h"

#include "platform.h"

AWDMaterial::AWDMaterial(const char *name, awd_uint16 name_len) : 
    AWDBlock(SIMPLE_MATERIAL),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->type = AWD_MATTYPE_UNDEFINED;
    this->isCreated = false;//will be true, once the mtl is converted to awd

    this->mappingChannel = 0;
    this->secondMappingChannel = 0;

    this->color = 0xffffff;

    this->texture = NULL;
    this->normalTexture = NULL;
    this->multiPass = false;
    this->repeat = true;
    this->smooth = true;
    this->mipmap = false;
    this->both_sides = false;
    this->premultiplied = false;
    this->alpha_blending = false;
    this->blendMode = 0;
    this->alpha = 1.0;
    this->alpha_threshold = 0.0f;
    this->ambientStrength = 1.0f;
    this->ambientColor = 0xffffff;
    this->ambientTexture = NULL;
    this->specularStrength = 1.0f;
    this->glossStrength = 50;
    this->specularColor = 0xffffff;
    this->specTexture = NULL;
    this->lightPicker = NULL;
    
    this->first_method = NULL;
    this->last_method = NULL;
    this->num_methods = 0;

    this->is_faceted = false;

    this->uv_transform_mtx = NULL;

    this->effectMethods = new AWDBlockList();
    this->materialClones = new AWDBlockList(false);// the materialClones will not be added into any of the awd-blocklists
}
    
AWDMaterial::~AWDMaterial()
{
    delete this->materialClones;
    if(this->effectMethods!=NULL)
        delete this->effectMethods;
    this->texture = NULL;
    this->ambientTexture = NULL;
    this->specTexture = NULL;
    this->normalTexture = NULL;
    this->effectMethods= NULL;
    
    this->type = AWD_MATTYPE_UNDEFINED;
    AWD_mat_method *cur;
    cur = this->first_method;
    while (cur) {
        AWD_mat_method *next=cur->next;
        AWDShadingMethod * thisShadingMethod=cur->method;
        if (thisShadingMethod!=NULL)
            delete thisShadingMethod;
        free(cur);
        cur = next;
    }
    if (this->uv_transform_mtx!=NULL) {
        free(this->uv_transform_mtx);
        this->uv_transform_mtx = NULL;
    }
}


void
AWDMaterial::resetShadingMethods()
{
    AWD_mat_method *cur;
    cur = this->first_method;
    while (cur) {
        AWD_mat_method *next=cur->next;
        AWDShadingMethod * thisShadingMethod=cur->method;
        if (thisShadingMethod!=NULL)
            delete thisShadingMethod;
        free(cur);
        cur = next;
    }
    this->first_method = NULL;
    this->last_method = NULL;
    this->num_methods = 0;
}
AWD_mat_type
AWDMaterial::get_type()
{
    return this->type;
}

void
AWDMaterial::set_type(AWD_mat_type type)
{
    this->type = type;
}


AWDBlockList *
AWDMaterial::get_materialClones()
{
    return this->materialClones;
}
AWDBlockList *
AWDMaterial::get_effectMethods()
{
    return this->effectMethods;
}

void
AWDMaterial::set_effectMethods(AWDBlockList *effectMethods)
{
    if(this->effectMethods!=NULL)
        delete this->effectMethods;
    this->effectMethods = effectMethods;
}

bool 
AWDMaterial::get_isCreated()
{
    return this->isCreated;
}
void
AWDMaterial::set_isCreated(bool isCreated)
{
    this->isCreated = isCreated;
}
awd_float64 *
AWDMaterial::get_uv_transform_mtx()
{
    return this->uv_transform_mtx;
}
void
AWDMaterial::set_uv_transform_mtx(awd_float64 * new_uv_transform_mtx)
{
    if (this->uv_transform_mtx!=NULL)
        free(this->uv_transform_mtx);
    this->uv_transform_mtx = (double *)malloc(6*sizeof(double));
    this->uv_transform_mtx[0] = new_uv_transform_mtx[0];
    this->uv_transform_mtx[1] = new_uv_transform_mtx[1];
    this->uv_transform_mtx[2] = new_uv_transform_mtx[2];
    this->uv_transform_mtx[3] = new_uv_transform_mtx[3];
    this->uv_transform_mtx[4] = new_uv_transform_mtx[4];
    this->uv_transform_mtx[5] = new_uv_transform_mtx[5];
}

bool 
AWDMaterial::get_is_faceted()
{
    return this->is_faceted;
}
void
AWDMaterial::set_is_faceted(bool isfaceted)
{
    this->is_faceted = isfaceted;
}

int 
AWDMaterial::get_mappingChannel()
{
    return this->mappingChannel;
}
void
AWDMaterial::set_mappingChannel(int mappingChannel)
{
    this->mappingChannel = mappingChannel;
}
int 
AWDMaterial::get_secondMappingChannel()
{
    return this->secondMappingChannel;
}
void
AWDMaterial::set_secondMappingChannel(int secondMappingChannel)
{
    this->secondMappingChannel = secondMappingChannel;
}

void
AWDMaterial::add_method(AWDShadingMethod *method)
{
    AWD_mat_method *item;

    item = (AWD_mat_method *)malloc(sizeof(AWD_mat_method));
    item->method = method;
    item->next = NULL;

    if (!this->first_method)
        this->first_method = item;
    else
        this->last_method->next = item;

    this->last_method = item;
    this->last_method->next = NULL;

    this->num_methods++;
}


awd_color
AWDMaterial::get_color()
{
    return this->color;
}
void
AWDMaterial::set_color(awd_color color)
{
    this->color = color;
}

AWDBitmapTexture *
AWDMaterial::get_texture()
{
    return this->texture;
}
void
AWDMaterial::set_texture(AWDBitmapTexture *texture)
{
    this->texture = texture;
}

AWDBitmapTexture *
AWDMaterial::get_normalTexture()
{
    return this->normalTexture;
}
void
AWDMaterial::set_normalTexture(AWDBitmapTexture *normalTexture)
{
    this->normalTexture = normalTexture;
}
        
bool 
AWDMaterial::get_multiPass()
{
    return this->multiPass;
}
void
AWDMaterial::set_multiPass(bool multiPass)
{
    this->multiPass = multiPass;
}
        
bool 
AWDMaterial::get_smooth()
{
    return this->smooth;
}
void
AWDMaterial::set_smooth(bool smooth)
{
    this->smooth = smooth;
}
        
bool 
AWDMaterial::get_mipmap()
{
    return this->mipmap;
}
void
AWDMaterial::set_mipmap(bool mipmap)
{
    this->mipmap = mipmap;
}
        
bool 
AWDMaterial::get_both_sides()
{
    return this->both_sides;
}
void
AWDMaterial::set_both_sides(bool both_sides)
{
    this->both_sides = both_sides;
}

bool 
AWDMaterial::get_premultiplied()
{
    return this->premultiplied;
}
void
AWDMaterial::set_premultiplied(bool premultiplied)
{
    this->premultiplied = premultiplied;
}

int 
AWDMaterial::get_blendMode()
{
    return this->blendMode;
}
void
AWDMaterial::set_blendMode(int blendMode)
{
    this->blendMode = blendMode;
}

awd_float32 
AWDMaterial::get_alpha()
{
    return this->alpha;
}
void
AWDMaterial::set_alpha(awd_float32 alpha)
{
    this->alpha = alpha;
}

bool 
AWDMaterial::get_alpha_blending()
{
    return this->alpha_blending;
}
void
AWDMaterial::set_alpha_blending(bool alpha_blending)
{
    this->alpha_blending = alpha_blending;
}

awd_float32 
AWDMaterial::get_alpha_threshold()
{
    return this->alpha_threshold;
}
void
AWDMaterial::set_alpha_threshold(awd_float32 alpha_threshold)
{
    this->alpha_threshold = alpha_threshold;
}

bool 
AWDMaterial::get_repeat()
{
    return this->repeat;
}
void
AWDMaterial::set_repeat(bool repeat)
{
    this->repeat = repeat;
}

awd_float32 
AWDMaterial::get_ambientStrength()
{
    return this->ambientStrength;
}
void
AWDMaterial::set_ambientStrength(awd_float32 ambientStrength)
{
    this->ambientStrength = ambientStrength;
}

awd_color
AWDMaterial::get_ambientColor()
{
    return this->ambientColor;
}
void
AWDMaterial::set_ambientColor(awd_color ambientColor)
{
    this->ambientColor = ambientColor;
}

AWDBitmapTexture *
AWDMaterial::get_ambientTexture()
{
    return this->ambientTexture;
}
void
AWDMaterial::set_ambientTexture(AWDBitmapTexture *ambientTexture)
{
    this->ambientTexture = ambientTexture;
}

awd_float32 
AWDMaterial::get_specularStrength()
{
    return this->specularStrength;
}
void
AWDMaterial::set_specularStrength(awd_float32 specularStrength)
{
    this->specularStrength = specularStrength;
}

awd_uint16 
AWDMaterial::get_glossStrength()
{
    return this->glossStrength;
}
void
AWDMaterial::set_glossStrength(awd_uint16 glossStrength)
{
    this->glossStrength = glossStrength;
}

awd_color
AWDMaterial::get_specularColor()
{
    return this->specularColor;
}
void
AWDMaterial::set_specularColor(awd_color specularColor)
{
    this->specularColor = specularColor;
}

AWDBitmapTexture *
AWDMaterial::get_specTexture()
{
    return this->specTexture;
}
void
AWDMaterial::set_specTexture(AWDBitmapTexture *specTexture)
{
    this->specTexture = specTexture;
}

AWDLightPicker *
AWDMaterial::get_lightPicker()
{
    return this->lightPicker;
}

void
AWDMaterial::set_lightPicker(AWDLightPicker *lightPicker_block)
{
    this->lightPicker = lightPicker_block;
}



void
AWDMaterial::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    if (this->type == AWD_MATTYPE_COLOR) {
        if (this->color != 0xffffff) {
            AWD_field_ptr col_val;
            col_val.v = malloc(sizeof(awd_uint32));
            *col_val.col = this->color;
            this->properties->set(PROP_MAT_COLOR, col_val, sizeof(awd_color), AWD_FIELD_COLOR);
        }
    }
    else {
        if (this->texture) {
            if (this->texture->get_tex_type()!=UNDEFINEDTEXTYPE){
                this->texture->prepare_and_add_with_dependencies(export_list);			
                AWD_field_ptr tex_val;
                tex_val.v = malloc(sizeof(awd_baddr));
                *tex_val.addr = this->texture->get_addr();
                this->properties->set(PROP_MAT_TEXTURE, tex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
            }
        }
        if (this->ambientTexture) {
            if (this->ambientTexture->get_tex_type()!=UNDEFINEDTEXTYPE){
                this->ambientTexture->prepare_and_add_with_dependencies(export_list);
                AWD_field_ptr ambientTex_val;
                ambientTex_val.v = malloc(sizeof(awd_baddr));
                *ambientTex_val.addr = this->ambientTexture->get_addr();
                this->properties->set(PROP_MAT_AMBIENTTEXTURE, ambientTex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
            }
        }
        
    }
    
    // create wrapper-methods for all effectMethods (wrapper methods only consist of awdID thats pointing to the EffectMethods-Block)
    AWDEffectMethod *block;
    AWDBlockIterator *it;
    it = new AWDBlockIterator(this->effectMethods);
    while ((block = (AWDEffectMethod *) it->next()) != NULL) {
        block->prepare_and_add_with_dependencies(export_list);
        AWDShadingMethod * newWrapperMethod = new AWDShadingMethod(AWD_EFFECTMETHOD_WRAPPER);
        AWD_field_ptr effectBlock_val;
        effectBlock_val.v = malloc(sizeof(awd_baddr));
        *effectBlock_val.addr = block->get_addr();
        newWrapperMethod->get_shading_props()->set(PROPS_BADDR1, effectBlock_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
        this->add_method(newWrapperMethod);
    }
    delete it;
    
    //tell all shading-methods to export their dependencies...
    AWD_mat_method *cur;
    cur = this->first_method;
    while (cur) {
        cur->method->prepare_and_add_dependencies(export_list);
        cur = cur->next;
    }

    // this properties can be set for texture and for color materials
    if (this->lightPicker!=NULL) {
        this->lightPicker->prepare_and_add_with_dependencies(export_list);		
        AWD_field_ptr lightPicker_val;
        lightPicker_val.v = malloc(sizeof(awd_baddr));
        *lightPicker_val.addr = this->lightPicker->get_addr();
        this->properties->set(PROP_MAT_LIGHTPICKER, lightPicker_val, sizeof(awd_baddr), AWD_FIELD_BADDR);		
    }
    if (this->normalTexture) {
        if (this->normalTexture->get_tex_type()!=UNDEFINEDTEXTYPE){
            this->normalTexture->prepare_and_add_with_dependencies(export_list);		
            AWD_field_ptr normaltex_val;
            normaltex_val.v = malloc(sizeof(awd_baddr));
            *normaltex_val.addr = this->normalTexture->get_addr();
            this->properties->set(PROP_MAT_NORMALTEXTURE, normaltex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
        }
    }
    if (this->specTexture) {
        if (this->specTexture->get_tex_type()!=UNDEFINEDTEXTYPE){
            this->specTexture->prepare_and_add_with_dependencies(export_list);
            AWD_field_ptr specTex_val;
            specTex_val.v = malloc(sizeof(awd_baddr));
            *specTex_val.addr = this->specTexture->get_addr();
             this->properties->set(PROP_MAT_SPECULARTEXTURE, specTex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
        }
    }
    
    if (this->multiPass) {
        AWD_field_ptr rep_val2;
        rep_val2.v = malloc(sizeof(awd_uint8));
        *rep_val2.ui8 = (awd_uint8)1;
        this->properties->set(PROP_MAT_SPECIAL_ID, rep_val2, sizeof(awd_uint8), AWD_FIELD_UINT8);
    }
    if (this->repeat) {
        AWD_field_ptr rep_val;
        rep_val.v = malloc(sizeof(awd_bool));
        *rep_val.b = AWD_TRUE;
        this->properties->set(PROP_MAT_REPEAT, rep_val, sizeof(awd_bool), AWD_FIELD_BOOL);
    }
    if (!this->smooth) {
        AWD_field_ptr rep_val;
        rep_val.v = malloc(sizeof(awd_bool));
        *rep_val.b = AWD_FALSE;
        this->properties->set(PROP_MAT_SMOOTH, rep_val, sizeof(awd_bool), AWD_FIELD_BOOL);
    }
    if (!this->mipmap) {
        AWD_field_ptr rep_val;
        rep_val.v = malloc(sizeof(awd_bool));
        *rep_val.b = AWD_FALSE;
        this->properties->set(PROP_MAT_MIPMAP, rep_val, sizeof(awd_bool), AWD_FIELD_BOOL);
    }
    if (this->both_sides) {
        AWD_field_ptr rep_val;
        rep_val.v = malloc(sizeof(awd_bool));
        *rep_val.b = AWD_TRUE;
        this->properties->set(PROP_MAT_BOTHSIDES, rep_val, sizeof(awd_bool), AWD_FIELD_BOOL);
    }
    if (this->alpha != 1.0f) {
       AWD_field_ptr th_val;
       th_val.v = malloc(sizeof(awd_float64));
       *th_val.f64 = this->alpha;
       this->properties->set(PROP_MAT_ALPHA, th_val, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }

    if (this->alpha_blending) {
       AWD_field_ptr trans_val;
       trans_val.v = malloc(sizeof(awd_bool));
       *trans_val.b = AWD_TRUE;
       this->properties->set(PROP_MAT_ALPHA_BLENDING, trans_val, sizeof(awd_bool), AWD_FIELD_BOOL);
    }
    if (this->alpha_threshold != 0.0f) {
       AWD_field_ptr th_val;
       th_val.v = malloc(sizeof(awd_float64));
       *th_val.f64 = this->alpha_threshold;
       this->properties->set(PROP_MAT_ALPHA_THRESHOLD, th_val, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }
    if (this->premultiplied) {
       AWD_field_ptr trans_val;
       trans_val.v = malloc(sizeof(awd_bool));
       *trans_val.b = AWD_TRUE;
       this->properties->set(PROP_MAT_APLHA_PREMULTIPLIED, trans_val, sizeof(awd_bool), AWD_FIELD_BOOL);
    }
    if (this->blendMode!=0) {
       AWD_field_ptr trans_val;
       trans_val.v = malloc(sizeof(awd_uint8));
       *trans_val.ui8 = this->blendMode;
       this->properties->set(PROP_MAT_BLENDMODE, trans_val, sizeof(awd_uint8), AWD_FIELD_UINT8);
    }
    //TODO: fill in default color
    if (this->specularColor != 0xffffff) {
       AWD_field_ptr col_val;
       col_val.v = malloc(sizeof(awd_color));
       *col_val.col = this->specularColor;
       this->properties->set(PROP_MAT_SPECULARCOLOR, col_val, sizeof(awd_color), AWD_FIELD_COLOR);
    }
    //TODO: fill in default color
    if (this->ambientColor != 0xffffff) {
        AWD_field_ptr col_val;
        col_val.v = malloc(sizeof(awd_color));
        *col_val.col = this->ambientColor;
        this->properties->set(PROP_MAT_AMBIENTCOLOR, col_val, sizeof(awd_color), AWD_FIELD_COLOR);
    }
    if (this->specularStrength != 1.0f) {
        AWD_field_ptr th_val;
        th_val.v = malloc(sizeof(awd_float64));
        *th_val.f64 = this->specularStrength;
        this->properties->set(PROP_MAT_SPECULARLEVEL, th_val, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }
    if (this->ambientStrength != 1.0f) {
        AWD_field_ptr th_val;
        th_val.v = malloc(sizeof(awd_float64));
        *th_val.f64 = this->ambientStrength;
        this->properties->set(PROP_MAT_AMBIENT_LEVEL, th_val, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }
    if (this->glossStrength != 50) {
        AWD_field_ptr th_val;
        th_val.v = malloc(sizeof(awd_float64));
        *th_val.f64 = (float)this->glossStrength;
        this->properties->set(PROP_MAT_GLOSS, th_val, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }
    
}

AWDMaterial*
AWDMaterial::get_material_for_lightPicker(AWDLightPicker * lightPicker)
{
    if (this->lightPicker==lightPicker){
        return this;
    }
    if (this->lightPicker==NULL){
        this->lightPicker=lightPicker;
        return this;
    }
    AWDMaterial * block;
    AWDBlockIterator * it;
    it = new AWDBlockIterator(this->materialClones);
    while ((block = (AWDMaterial *)it->next()) != NULL) {
        if (block->lightPicker==lightPicker){
            return block;
        }
    }
    delete it;
    AWDMaterial * clonedMat = new AWDMaterial(this->get_name(), this->get_name_length());
    clonedMat->set_type(this->type);
    clonedMat->set_is_faceted(this->is_faceted);
    clonedMat->set_ambientTexture(this->ambientTexture);
    clonedMat->set_texture(this->texture);
    clonedMat->set_specTexture(this->specTexture);
    clonedMat->set_normalTexture(this->normalTexture);
    clonedMat->set_mappingChannel(this->mappingChannel);//not needed ?
    clonedMat->set_secondMappingChannel(this->secondMappingChannel);//not needed ?
    clonedMat->color=this->color;
    clonedMat->alpha_threshold=this->alpha_threshold;
    clonedMat->alpha_blending=this->alpha_blending;
    clonedMat->repeat=this->repeat;
    clonedMat->ambientColor=this->ambientColor;
    clonedMat->specularColor=this->specularColor;
    clonedMat->specularStrength=this->specularStrength;
    clonedMat->ambientStrength=this->ambientStrength;
    clonedMat->glossStrength=this->glossStrength;
    clonedMat->specularStrength=this->specularStrength;
    clonedMat->effectMethods=this->effectMethods;
    clonedMat->first_method=this->first_method;
    clonedMat->last_method=this->last_method;
    clonedMat->num_methods=this->num_methods;
    clonedMat->set_multiPass(this->multiPass);

    clonedMat->set_lightPicker(lightPicker);
    
    this->materialClones->append(clonedMat);
    return clonedMat;
}


awd_uint32
AWDMaterial::calc_body_length(BlockSettings * curBlockSettings)
{
    AWD_mat_method *cur;
    awd_uint32 len;
    
    len = sizeof(awd_uint16) + this->get_name_length(); //name
    len += sizeof(awd_uint8); // type
    len += sizeof(awd_uint8); // method count
    
    len += this->calc_attr_length(true, true, curBlockSettings);

    //at this point, all effectmethods should be included as wrappermethods in the materials method-list 
    cur = this->first_method;
    while (cur) {
        len += cur->method->calc_method_length(curBlockSettings);
        cur = cur->next;
    }

    return len;
}

void
AWDMaterial::write_body(int fd,  BlockSettings * curBlockSettings)
{

    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    write(fd, &this->type, sizeof(awd_uint8));
    write(fd, &this->num_methods, sizeof(awd_uint8));

    this->properties->write_attributes(fd, curBlockSettings);
    
    AWD_mat_method *cur;
    cur = this->first_method;
    while (cur) {
        cur->method->write_method(fd, curBlockSettings);
        cur = cur->next;
    }

    this->user_attributes->write_attributes(fd, curBlockSettings);
}
