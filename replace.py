import os
import re

src_dir = r'c:\Users\kaaif\Documents\Github\Runtime-Terror\src'
for root, dirs, files in os.walk(src_dir):
    for file in files:
        if file.endswith('.cpp') or file.endswith('.h'):
            if file == 'constants.h':
                continue
            filepath = os.path.join(root, file)
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            new_content = re.sub(r'\bDrawText\s*\(', 'DrawVSText(', content)
            new_content = re.sub(r'\bMeasureText\s*\(', 'MeasureVSText(', content)
            
            if new_content != content:
                with open(filepath, 'w', encoding='utf-8') as f:
                    f.write(new_content)
                print(f"Updated {file}")
