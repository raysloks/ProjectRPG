import bpy
import struct
import bmesh

def read_gmdl(context, filepath):
    print("running read_gmdl...")
    
    scene = context.scene
        
    abs_filepath = bpy.path.abspath(filepath)
        
    with open(filepath, 'rb') as f:
        
        def read_string(f):
            length = struct.unpack('<I', f.read(4))
            return f.read(length[0]).decode()
        
        mesh = bpy.data.meshes.new('mesh')
        obj = bpy.data.objects.new('object', mesh)
        
        context.collection.objects.link(obj)
        
        texverts = []
        
        bm = bmesh.new()
        
        uv_layer = bm.loops.layers.uv.verify()
        
        face_vertex = 0
        byte = f.read(1)
        while byte:
            prefix = byte
            if prefix == b'v':
                print('v')
                bm.verts.new(struct.unpack('<3f', f.read(12)))
                bm.verts.ensure_lookup_table()
            if prefix == b'n':
                print('n')
                bm.verts[-1].normal = struct.unpack('<3f', f.read(12))
            if prefix == b'f':
                print('f')
                bm.faces.new([bm.verts[i] for i in struct.unpack('<3H', f.read(6))])
                bm.faces.ensure_lookup_table()
                face_vertex = 0
            if prefix == b'F':
                print('F')
                bm.faces.new([bm.verts[i] for i in struct.unpack('<3I', f.read(12))])
                bm.faces.ensure_lookup_table()
                face_vertex = 0
            if prefix == b'u':
                print('u')
                texverts.append(struct.unpack('<2f', f.read(8)))
            if prefix == b't':
                print('t')
                bm.faces[-1].loops[face_vertex][uv_layer].uv = texverts[struct.unpack('<H', f.read(2))[0]]
                face_vertex += 1
            if prefix == b'T':
                print('T')
                bm.faces[-1].loops[face_vertex][uv_layer].uv = texverts[struct.unpack('<I', f.read(4))[0]]
                face_vertex += 1
            if prefix == b'i':
                print('i')
                f.read(6)
            if prefix == b'I':
                print('I')
                f.read(12)
            if prefix == b'w':
                print('w')
                f.read(8)
            if prefix == b'm':
                print('m')
            if prefix == b'M':
                print('M')
                read_string(f)
            if prefix == b'o':
                print('o')
                read_string(f)
            byte = f.read(1)
        
        bm.to_mesh(mesh)
        bm.free()
        
        f.close()

        return {'FINISHED'}

# ImportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ImportHelper
from bpy.props import StringProperty


class ImportGMDL(bpy.types.Operator, ImportHelper):
    '''Imports a .gmdl file.'''
    bl_idname = "import_mesh.gmdl"
    bl_label = "Import GMDL"

    # ImportHelper mixin class uses this
    filename_ext = ".gmdl"

    filter_glob : StringProperty(default="*.gmdl", options={'HIDDEN'})

    def execute(self, context):
        return read_gmdl(context, self.filepath)


# Only needed if you want to add into a dynamic menu
def menu_func_import(self, context):
    self.layout.operator(ImportGMDL.bl_idname, text="Game 3d Model (.gmdl)")


def register():
    bpy.utils.register_class(ImportGMDL)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)


def unregister():
    bpy.utils.unregister_class(ImportGMDL)
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)


if __name__ == "__main__":
    register()

    # test call
    bpy.ops.import_mesh.gmdl('INVOKE_DEFAULT')