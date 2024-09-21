import { FC, useContext, useEffect, useRef, useState } from "react"
import { Button } from "../ui/button"
import { DropdownMenu, DropdownMenuContent, DropdownMenuTrigger } from "../ui/dropdown-menu"
import { Input } from "../ui/input"
import { Tabs, TabsList, TabsTrigger } from "../ui/tabs"
import { ModelIcon } from "./model-icon"
import { ModelOption } from "./model-option"
import api from "../../app/phoenix_services/api"
import {IconCheck} from "@tabler/icons-react";

interface ModelSelectProps {
  selectedModelId: string
  onSelectModel: (modelId: string) => void
}

interface APIModel {
  id: number
  model_name: string
  path: string
}

export const ModelSelect: FC<ModelSelectProps> = ({ selectedModelId, onSelectModel }) => {
  const inputRef = useRef<HTMLInputElement>(null)
  const triggerRef = useRef<HTMLButtonElement>(null)

  const [isOpen, setIsOpen] = useState(false)
  const [search, setSearch] = useState("")
  const [tab, setTab] = useState<"hosted" | "local">("local")
  const [models, setModels] = useState<APIModel[]>([])
  const [loading, setLoading] = useState(true)

  useEffect(() => {
    // Fetch the models from your API
    const fetchModels = async () => {
      try {
        const response = await api.get("/model/local")
        setModels(response.data)
        setLoading(false)
      } catch (error) {
        console.error("Failed to fetch models", error)
        setLoading(false)
      }
    }

    fetchModels()
  }, [])

  useEffect(() => {
    if (isOpen) {
      setTimeout(() => {
        inputRef.current?.focus()
      }, 100) // Focus on search input when dropdown opens
    }
  }, [isOpen])

  const handleSelectModel = (modelId: string) => {
    onSelectModel(modelId)
    setIsOpen(false)
  }

  const filteredModels = models.filter(model => model.model_name.toLowerCase().includes(search.toLowerCase()))

  if (loading) return <div>Loading models...</div>

  return (
    <DropdownMenu
      open={isOpen}
      onOpenChange={isOpen => {
        setIsOpen(isOpen)
        setSearch("")
      }}
    >
      <DropdownMenuTrigger className="bg-background w-full justify-start border-2 px-3 py-5" asChild>
        <Button ref={triggerRef} className="flex items-center justify-between" variant="ghost">
          <div className="flex items-center">
            {selectedModelId ? (
              <>
                <ModelIcon provider="custom" width={26} height={26} />
                <div className="ml-2 flex items-center">
                  {models.find(model => model.id.toString() === selectedModelId)?.model_name}
                </div>
              </>
            ) : (
              <div className="flex items-center">Select a model</div>
            )}
          </div>
        </Button>
      </DropdownMenuTrigger>

      <DropdownMenuContent className="space-y-2 overflow-auto p-2" style={{ width: triggerRef.current?.offsetWidth }} align="start">
        <Tabs value={tab} onValueChange={(value: any) => setTab(value)}>
          <TabsList className="grid grid-cols-2">
            <TabsTrigger value="hosted">Hosted</TabsTrigger>
            <TabsTrigger value="local">Local</TabsTrigger>
          </TabsList>
        </Tabs>

        <Input
          ref={inputRef}
          className="w-full"
          placeholder="Search models..."
          value={search}
          onChange={e => setSearch(e.target.value)}
        />

        <div className="max-h-[300px] overflow-auto">
          {filteredModels.map(model => (
            <div key={model.id} className="flex items-center space-x-1">
              {selectedModelId === model.id.toString() && <IconCheck className="ml-2" size={32} />}
              <ModelOption
                key={model.id}
                model={{
                  modelId: model.id.toString(),
                  modelName: model.model_name,
                  provider: "custom",
                  platformLink: "", // You can update this with the actual platform link if available
                  imageInput: false, // Or set to true based on your logic
                }}
                onSelect={() => handleSelectModel(model.id.toString())}
              />
            </div>
          ))}
        </div>
      </DropdownMenuContent>
    </DropdownMenu>
  )
}
