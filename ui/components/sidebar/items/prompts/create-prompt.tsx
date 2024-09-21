import { SidebarCreateItem } from "@/components/sidebar/items/all/sidebar-create-item"
import { Input } from "@/components/ui/input"
import { Label } from "@/components/ui/label"
import { TextareaAutosize } from "@/components/ui/textarea-autosize"
import { ChatbotUIContext } from "@/context/context"
import { PROMPT_NAME_MAX } from "@/db/limits"
import { FC, useContext, useState } from "react"
import api from "../../../../app/phoenix_services/api" // Import your axios instance

interface CreatePromptProps {
  isOpen: boolean
  onOpenChange: (isOpen: boolean) => void
}

export const CreatePrompt: FC<CreatePromptProps> = ({
                                                      isOpen,
                                                      onOpenChange
                                                    }) => {
  const { profile, selectedWorkspace } = useContext(ChatbotUIContext)
  const [isTyping, setIsTyping] = useState(false)
  const [name, setName] = useState("")
  const [content, setContent] = useState("")
  const [loading, setLoading] = useState(false)

  const createPrompt = async () => {
    if (!profile || !selectedWorkspace) return

    setLoading(true)
    try {
      const response = await api.post('/prompt', {
        folder_id: selectedWorkspace.fol, // Adjust folder_id as needed
        sharing: "private",
        content,
        name,
      })
      console.log("Prompt created:", response.data)
      onOpenChange(false) // Close the sidebar on success
    } catch (error) {
      console.error("Error creating prompt:", error)
    } finally {
      setLoading(false)
    }
  }

  if (!profile || !selectedWorkspace) return null

  return (
    <SidebarCreateItem
      contentType="prompts"
      isOpen={isOpen}
      isTyping={isTyping}
      onOpenChange={onOpenChange}
      createState={null}
      onCreate={createPrompt} // Use the createPrompt function for creation
      renderInputs={() => (
        <>
          <div className="space-y-1">
            <Label>Name</Label>
            <Input
              placeholder="Prompt name..."
              value={name}
              onChange={e => setName(e.target.value)}
              maxLength={PROMPT_NAME_MAX}
              onCompositionStart={() => setIsTyping(true)}
              onCompositionEnd={() => setIsTyping(false)}
            />
          </div>

          <div className="space-y-1">
            <Label>Prompt</Label>
            <TextareaAutosize
              placeholder="Prompt content..."
              value={content}
              onValueChange={setContent}
              minRows={6}
              maxRows={20}
              onCompositionStart={() => setIsTyping(true)}
              onCompositionEnd={() => setIsTyping(false)}
            />
          </div>

          {loading && <p>Creating prompt...</p>}
        </>
      )}
    />
  )
}
