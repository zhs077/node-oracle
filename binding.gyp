{
    'conditions': [
      ['OS=="win"', {
        'targets': [
          {
            'target_name': 'oracle_client',
			 'include_dirs':[
	  'src/oci',
	  'src/un/include',
	  'src/un/src/win32'
	  ],
      'sources': ['src/nodejs/oracle_client.cc','src/nodejs/Connection.cc','src/third_party/CodingConv/encodeutil.cpp',
	 'src/core/plateform_public.cpp','src/nodejs/nodefunction.cc','src/core/otlPool.cpp','src/nodejs/ConnctionPool.cc',
	 	'src/un/src/win32/thread.c','src/un/src/win32/winapi.c','src/un/src/win32/error.c'
	  ],
	  'link_settings': {
        'libraries': [
          '../src/lib/oci'
        ],
      },
             
          },
        ],
      }],
	 ['OS=="linux"', {
        'targets': [
          {
            'target_name': 'oracle_client',
			 'include_dirs':[
	  'src/oci',
	  'src/un/include',
	  ],
      'sources': ['src/nodejs/oracle_client.cc','src/nodejs/Connection.cc','src/third_party/CodingConv/encodeutil.cpp',
	 'src/core/plateform_public.cpp','src/nodejs/nodefunction.cc','src/core/otlPool.cpp','src/nodejs/ConnctionPool.cc',
	 	'src/un/src/linux/thread.c'
	  ],
	  'variables':{
	  			"oci_lib_dir%": "<!(echo $ORACLE_HOME)"
	  },
	  'libraries': [ '-lclntsh','-lpthread' ],
	'link_settings': {'libraries': [ '-L<(oci_lib_dir)/lib'] },
	'cflags':['-fexceptions'],
	'cflags_cc':['-fexceptions']
             
          },
        ],
      }]
     
	],
	
 	
}
