import bpy
import struct
import re

def getIndex(name):
    n = -1
    for ob in bpy.data.objects:
        try:
            if n==-1:
                n = ob.vertex_groups[name].index
        except:
            print("No vertex group.")
    if n==-1:
        print("ERROR: No vertex group index found!")
    return n

def write_anim(context, filepath):
    print("running write_anim...")
    f = open(filepath, 'wb')
    
    arm = bpy.data.armatures[0]
    f.write(struct.pack('i',len(arm.bones)))
    for bone in arm.bones:
        f.write(struct.pack('i', getIndex(bone.name)))
        f.write(struct.pack('i',len(bone.name)))
        for x in bone.name:
            f.write(struct.pack('<B',ord(x)))
        if bone.parent==None:
            f.write(struct.pack('i', -1))
        else:
            f.write(struct.pack('i', getIndex(bone.parent.name)))
        f.write(struct.pack('4f', bone.matrix_local[0][0], bone.matrix_local[0][1], bone.matrix_local[0][2], bone.matrix_local[0][3]))
        f.write(struct.pack('4f', bone.matrix_local[1][0], bone.matrix_local[1][1], bone.matrix_local[1][2], bone.matrix_local[1][3]))
        f.write(struct.pack('4f', bone.matrix_local[2][0], bone.matrix_local[2][1], bone.matrix_local[2][2], bone.matrix_local[2][3]))
        f.write(struct.pack('4f', bone.matrix_local[3][0], bone.matrix_local[3][1], bone.matrix_local[3][2], bone.matrix_local[3][3]))
        
    actions = [act for act in bpy.data.actions if act.use_fake_user]
    f.write(struct.pack('i',len(actions)))
    for act in actions:
        if act.use_fake_user:
            f.write(struct.pack('i',len(act.name)))
            for x in act.name:
                f.write(struct.pack('<B',ord(x)))
            f.write(struct.pack('i',len(act.groups)))
            for group in act.groups:
                n = getIndex(group.name)
                f.write(struct.pack('i', n))
                f.write(struct.pack('i',len(group.channels)))
                prop_count = 0
                for channel in group.channels:
                    default_channel = 0
                    if (channel.data_path.rfind('location')==len(channel.data_path)-8):
                        f.write(struct.pack('<B',channel.array_index))
                        default_channel = 1
                    if (channel.data_path.rfind('rotation_quaternion')==len(channel.data_path)-19):
                        f.write(struct.pack('<B',3+channel.array_index))
                        default_channel = 1
                    if (channel.data_path.rfind('scale')==len(channel.data_path)-5):
                        f.write(struct.pack('<B',7+channel.array_index))
                        default_channel = 1
                    if default_channel == 0:
                        f.write(struct.pack('<B',10 + prop_count))
                        prop_count += 1
                        m = re.search('.*\[\"(.*)\"\]', channel.data_path)
                        name = m.group(1)
                        f.write(struct.pack('i',len(name)))
                        for x in name:
                            f.write(struct.pack('<B',ord(x)))
                    f.write(struct.pack('i',len(channel.keyframe_points)))
                    for key in channel.keyframe_points:
                        f.write(struct.pack('2f',key.co[0],key.co[1]))
    
    f.close()

    return {'FINISHED'}

# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty


class ExportANIM(bpy.types.Operator, ExportHelper):
    '''Exports a .anim file.'''
    bl_idname = "export.anim"
    bl_label = "Export ANIM"

    # ExportHelper mixin class uses this
    filename_ext = ".anim"

    filter_glob = StringProperty(default="*.anim", options={'HIDDEN'})

    @classmethod
    def poll(cls, context):
        return context.active_object != None

    def execute(self, context):
        return write_anim(context, self.filepath)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportANIM.bl_idname, text="Animation Data (.anim)")


def register():
    bpy.utils.register_class(ExportANIM)
    bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportANIM)
    bpy.types.INFO_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # test call
    bpy.ops.export.anim('INVOKE_DEFAULT')