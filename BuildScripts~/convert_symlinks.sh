
ESCAPED_PATH=${1}
TEMP_SYMLINK="${ESCAPED_PAT}Temp" 

echo
echo --------------------------------------------------------------------------
echo Converting symlinks to copies: $1 
echo --------------------------------------------------------------------------
mv "${ESCAPED_PATH}" "$TEMP_SYMLINK"
rsync "${TEMP_SYMLINK}/" "${ESCAPED_PATH}/" -a --copy-links -v
rm "${TEMP_SYMLINK}"
